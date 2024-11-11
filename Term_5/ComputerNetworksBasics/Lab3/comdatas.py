import sys
import random
from time import sleep

from PyQt6.QtSerialPort import QSerialPortInfo, QSerialPort
from PyQt6.QtWidgets import QMainWindow, QApplication, QMessageBox
from PyQt6.QtCore import QIODeviceBase

from cominfo_window import Ui_ComsInterraction_MW
from PyQt6.QtGui import QIcon, QTextCursor, QTextCharFormat, QColor


class ComInfoWindow(QMainWindow, Ui_ComsInterraction_MW):
    def __init__(self):
        super(QMainWindow, self).__init__()
        self.setupUi(self)
        # self.setWindowIcon(QIcon("port_i.jpg"))

        self.bitsNumComboBox.addItem("5", QSerialPort.DataBits.Data5)
        self.bitsNumComboBox.addItem("6", QSerialPort.DataBits.Data6)
        self.bitsNumComboBox.addItem("7", QSerialPort.DataBits.Data7)
        self.bitsNumComboBox.addItem("8", QSerialPort.DataBits.Data8)

        self.actionExit.triggered.connect(self.close)
        self.actionRenewComsNum.triggered.connect(self.renewAvailableComsList)
        self.actionCleanOutput.triggered.connect(self.outputTextBrowser.clear)
        self.inputTextEdit.textChanged.connect(self.check_for_enter)
        self.comNumComboBox.currentIndexChanged.connect(self.changeCurrCom)

        self.port_is_chosen = False
        self.serialPort = QSerialPort()
        self.serialPort.setDataBits(QSerialPort.DataBits.Data8)
        self.bitsNumComboBox.setCurrentIndex(3)
        self.serialPort.setBaudRate(9600)
        self.statusTextBrowser.append("The BaudRate is set to 9600 bits per second")
        self.serialPort.setParity(QSerialPort.Parity.NoParity)
        self.serialPort.setStopBits(QSerialPort.StopBits.OneStop)
        self.serialPort.setFlowControl(QSerialPort.FlowControl.NoFlowControl)

        self.renewAvailableComsList()
        self.serialPort.readyRead.connect(self.readDatas)

        # self.comNumComboBox.currentIndexChanged.connect(self.changeCurrCom)
        self.bitsNumComboBox.currentIndexChanged.connect(self.changeBitsNum)

    def closeEvent(self, event):
        if self.port_is_chosen:
            self.serialPort.close()
        print("Application is closed")

    def renewAvailableComsList(self):
        curr_com = self.comNumComboBox.currentText()

        self.comNumComboBox.currentIndexChanged.disconnect(self.changeCurrCom)
        self.comNumComboBox.clear()
        # self.comNumComboBox.currentIndexChanged.connect(self.changeCurrCom)

        com_ports = QSerialPortInfo.availablePorts()
        self.debugTextBrowser.append(str(len(com_ports)) + " COM ports found")
        if len(com_ports) == 0:
            QMessageBox.warning(None, "Warning", "No available COM Ports found")
            self.inputTextEdit.setReadOnly(True)
            self.comNumComboBox.setDisabled(True)
            self.bitsNumComboBox.setDisabled(True)
        else:
            for port in com_ports:
                self.comNumComboBox.addItem(port.portName()[3:])

            self.comNumComboBox.setCurrentText(curr_com)
            if self.port_is_chosen:
                self.debugTextBrowser.append("The current port is still COM" + curr_com)
            else:
                self.choseFirstAccessiblePort(com_ports)

        self.comNumComboBox.currentIndexChanged.connect(self.changeCurrCom)
        return com_ports

    def choseFirstAccessiblePort(self, available_ports_list):
        # if len(available_ports_list) != 0:
        for index, port in enumerate(available_ports_list):
            self.serialPort.setPortName(port.portName())
            # self.debugTextBrowser.append("Checking " + port.portName())
            if not self.serialPort.open(QIODeviceBase.OpenModeFlag.ReadWrite):
                self.debugTextBrowser.append(self.serialPort.portName() + " is busy")
            else:
                self.debugTextBrowser.append(self.serialPort.portName() + " is successfully opened")
                self.port_is_chosen = True
                self.statusbar.showMessage(self.serialPort.portName() + " is automatically chosen", 5000)
                self.comNumComboBox.setCurrentIndex(index)

                self.inputTextEdit.setReadOnly(False)
                self.comNumComboBox.setDisabled(False)
                self.bitsNumComboBox.setDisabled(False)
                break
        if not self.port_is_chosen:
            QMessageBox.warning(None, "Warning", "There is no COM Port that can be opened")
            self.inputTextEdit.setReadOnly(True)
            self.bitsNumComboBox.setDisabled(True)

    def check_for_enter(self):
        text = self.inputTextEdit.toPlainText()

        if text.endswith('\n'):
            self.inputTextEdit.clear()
            self.send_data(text[:-1])

        else:

            curs_pos = self.inputTextEdit.textCursor().position()
            new_text = ""
            for symb in text:
                if symb not in ['0', '1']:
                    curs_pos -= 1
                else:
                    new_text += symb

            self.inputTextEdit.textChanged.disconnect(self.check_for_enter)
            self.inputTextEdit.setPlainText(new_text)

            new_cursor = self.inputTextEdit.textCursor()
            new_cursor.setPosition(curs_pos)
            self.inputTextEdit.setTextCursor(new_cursor)

            self.inputTextEdit.textChanged.connect(self.check_for_enter)

    def send_data(self, input_str):
        print("Wrote:", bytes(input_str, 'utf-8'))
        self.debugTextBrowser.append("\nInput Data:" + input_str)

        while True:
            if len(input_str) == 0:
                break

            str_to_send = self.create_pocket(input_str[:23])
            self.debugTextBrowser.append("\nPocket Data:" + str_to_send)

            str_to_send = self.bit_staff_pocket(str_to_send)
            self.debugTextBrowser.append("Bits staffed pocket:" + str_to_send)

            input_str = input_str[23:]
            self.debugTextBrowser.append("Remain pocket Data:" + input_str)

            if self.serialPort.error() == QSerialPort.SerialPortError.NoError:
                bytes_written = self.serialPort.write(bytes(str_to_send, 'utf-8'))

                print("Sended:", bytes(str_to_send, 'utf-8'))

                self.statusTextBrowser.append("Send " + str(bytes_written) + " bytes")
            else:
                QMessageBox.critical(None, "Error", "Cannot write data in " + self.serialPort.portName())
                self.port_is_chosen = False
                self.serialPort.close()
                self.renewAvailableComsList()
                break
        self.statusTextBrowser.append("")

    def create_pocket(self, str_to_in_data):

        fcs_str = self.create_fcs(str_to_in_data)
        distr_data = self.make_distortion(str_to_in_data)

        data_str = distr_data + 'O' * (23 - len(distr_data))

        # data_str = str_to_in_data + 'O' * (23 - len(str_to_in_data))
        # fcs_str = self.create_fcs(data_str)
        # pocket_str = "00010111" + "0000" + format(int(self.serialPort.portName()[3:]), '04b') + data_str + "0"

        pocket_str = "00010111" + "0000" + format(int(self.serialPort.portName()[3:]), '04b') + data_str + fcs_str
        return pocket_str

    def create_fcs(self, data_str: str):
        fcs_str = ""

        for i in range(5):
            # i_pow = (2 ** i) - 1
            i_pow = (2 ** i)
            unos_sum = 0
            sym_to_add = '0'

            for j in range(i_pow, len(data_str) + 1):
                # k = j % (2 ** (i + 1))
                # print(str(i), str(j), "k =", str(k))
                # print("XXXX", str(j - 1), "XXXXX")

                if (j % (2 ** (i + 1))) >= (2 ** i):
                    # print("XXXX", str(j - 1), "XXXXX")
                    if data_str[j - 1] == '1':
                        unos_sum += 1

            # print("-----------------------------------------------")
            # print(str(i), "- Sum:", str(unos_sum))
            if unos_sum % 2 == 1:
                sym_to_add = '1'
            fcs_str += sym_to_add

        self.debugTextBrowser.append("Fcs: " + fcs_str)

        return fcs_str

    def make_distortion(self, data_str: str):
        distort_data = data_str
        if random.random() < 0.7:

            random_num = random.randint(0, len(data_str) - 1)
            if data_str[random_num] == '0':
                distort_data = data_str[:random_num] + '1' + data_str[random_num + 1:]
            else:
                distort_data = data_str[:random_num] + '0' + data_str[random_num + 1:]

            self.debugTextBrowser.append("Random num = " + str(random_num) + ". Distort_str: \"" + distort_data + "\"")

        return distort_data

    def bit_staff_pocket(self, poket_str):

        bit_staffed_str = poket_str

        # key: 0001 (1) 0 111
        # найти "0001"
        # . вставить после "1"
        # при чтении если после идет "0", то это флаг. если "1" - единицу удалить
        # 00010111 00000110AAAAAAAA011000100001000111

        ind = 8
        while True:
            # ind = bit_staffed_str.find("0001", ind)
            ind = bit_staffed_str.find("0001011", ind)
            if ind == -1:
                break
            print("\nPok before:", bit_staffed_str)
            ind += 7
            bit_staffed_str = bit_staffed_str[:ind] + '0' + bit_staffed_str[ind:]
            print("Pok after: ", bit_staffed_str)

        return bit_staffed_str

    def readDatas(self):
        data = self.serialPort.readAll()
        print("Received bytes:", data)
        if self.serialPort.error() == QSerialPort.SerialPortError.NoError:
            text_data = str(data, 'utf-8')
            self.debugTextBrowser.append("\nReceived data:" + text_data)
            self.statusTextBrowser.append("\nReceived " + str(data.size()) + " bytes:")

            ind_start = 0
            ind_next = 0
            full_data_str = ""
            while True:
                # ind_start = text_data.find("00010", ind_next)
                ind_start = text_data.find("00010111", ind_next)
                if ind_start == -1:
                    break
                # ind_next = text_data.find("00010", ind_start + 8)
                ind_next = text_data.find("00010111", ind_start + 8)
                if ind_next == -1:
                    ind_next = len(text_data)

                # self.print_bitf_str(text_data[ind_start:ind_next])
                self.debugTextBrowser.append("\nData to de bits staff:" + text_data[ind_start:ind_next])

                de_bits_staff_pock_str = self.de_bit_stf_str(text_data[ind_start:ind_next])
                self.debugTextBrowser.append("De Bits staffed pocket:" + de_bits_staff_pock_str)

                self.print_bitf_str(text_data[ind_start:ind_next])

                # пересчитываем fcs
                old_fcs = de_bits_staff_pock_str[-5:]
                new_fcs = self.create_fcs(de_bits_staff_pock_str[16:39])
                # исправляем
                if old_fcs != new_fcs:
                    print("DIFFERRRSSSS")
                    # изменить окно
                    self.fix_status_output_fcs(new_fcs)
                    # исправить дату в пакете
                    de_bits_staff_pock_str = self.fix_pocket_data_with_fcs(de_bits_staff_pock_str, old_fcs, new_fcs)

                full_data_str += self.get_data_from_pocket(de_bits_staff_pock_str)
                self.debugTextBrowser.append("Data from Pocket:" + de_bits_staff_pock_str)

            self.outputTextBrowser.append(full_data_str)  # (text_data[:-1])
            # cur_text = self.outputTextBrowser.toPlainText()
            # self.outputTextBrowser.setPlainText(cur_text + text_data)

        else:
            QMessageBox.critical(None, "Error", "Cannot read data from " + self.serialPort.portName())
            self.port_is_chosen = False
            self.serialPort.close()
            self.renewAvailableComsList()

    def print_bitf_str(self, bitstf_str: str):
        str_to_print = bitstf_str[:8] + " "

        ind_start = 8
        ind_next = 8
        while True:
            # ind_next = bitstf_str.find("00011", ind_start)
            ind_next = bitstf_str.find("00010110", ind_start)
            if ind_next == -1:
                break
            # ind_next += 4
            ind_next += 7
            str_to_print += bitstf_str[ind_start:ind_next] + "_(0)_"
            ind_start = ind_next + 1
        str_to_print += bitstf_str[ind_start:]

        # self.statusTextBrowser.append("**" + str_to_print + "**")
        # str_to_print = str_to_print[:-5] + " " + str_to_print[-5:]

        index = self.find_fcs_start_in_bitf_str_for_print(str_to_print)
        str_to_print = str_to_print[:index] + " " + str_to_print[index:]

        self.statusTextBrowser.append(str_to_print)

    def find_fcs_start_in_bitf_str_for_print(self, str_to_print: str):
        index = len(str_to_print) - 1
        num_from_end = 0

        while num_from_end < 5:
            if str_to_print[index] == '_':
                index -= 5
            num_from_end += 1
            index -= 1

        return index + 1

    def fix_status_output_fcs(self, new_fcs_str: str):
        curr_text = self.statusTextBrowser.toPlainText()
        txt_index = curr_text.rfind(" ") + 1
        print("Last sp ind:", str(txt_index), "Str:\"" + curr_text[txt_index:] + "\"")

        for i in range(5):
            if curr_text[txt_index] == '_':
                txt_index += 5
            curr_text = curr_text[:txt_index] + new_fcs_str[i] + curr_text[txt_index + 1:]
            txt_index += 1
        self.statusTextBrowser.setPlainText(curr_text)

    def fix_pocket_data_with_fcs(self, pocket_str: str, old_fcs: str, new_fcs: str):
        fixed_pocket = ""
        data_str = pocket_str[16:39]
        # print(data_str)

        err_symb_ind = 0
        for i in range(5):
            if old_fcs[i] != new_fcs[i]:
                err_symb_ind += (2 ** i)
        err_symb_ind -= 1
        if data_str[err_symb_ind] == '1':
            data_str = data_str[:err_symb_ind] + '0' + data_str[err_symb_ind + 1:]
        else:
            data_str = data_str[:err_symb_ind] + '1' + data_str[err_symb_ind + 1:]
        fixed_pocket = pocket_str[:16] + data_str + pocket_str[39:]

        return fixed_pocket

    def de_bit_stf_str(self, bitstf_str: str):
        pock_str = bitstf_str[:8]

        ind_start = 8
        ind_next = 8
        while True:
            ind_next = bitstf_str.find("0001011", ind_start)
            if ind_next == -1:
                break
            ind_next += 7 # 0001011 (0) 1pppp # 0001011 (0) 0
            pock_str += bitstf_str[ind_start:ind_next]
            ind_start = ind_next + 1
        pock_str += bitstf_str[ind_start:]

        return pock_str

    def get_data_from_pocket(self, pocket_str: str):
        # 00010111 0000 1111 12345678901234567890123 0
        pocket_str = pocket_str[16:39]
        ind = pocket_str.find('O')
        if ind == -1:
            ind = len(pocket_str)

        return pocket_str[:ind]

    def changeCurrCom(self):
        # print("IT IS CHANGE CUR COM")
        curr_com_name = self.serialPort.portName()
        self.serialPort.close()

        self.debugTextBrowser.append("Trying to change port on COM" + self.comNumComboBox.currentText())
        self.serialPort.setPortName("COM" + self.comNumComboBox.currentText())

        if self.serialPort.open(QIODeviceBase.OpenModeFlag.ReadWrite):
            self.debugTextBrowser.append(self.serialPort.portName() + " is successfully opened")
        else:
            QMessageBox.critical(None, "Error", self.serialPort.portName() + " cannot be opened in the current app")
            self.debugTextBrowser.append(self.serialPort.portName() + " cannot be opened")
            self.debugTextBrowser.append("Trying to reopen " + curr_com_name)
            self.serialPort.setPortName(curr_com_name)

            if self.serialPort.open(QIODeviceBase.OpenModeFlag.ReadWrite):
                self.debugTextBrowser.append(self.serialPort.portName() + " is successfully opened")

                self.comNumComboBox.currentIndexChanged.disconnect(self.changeCurrCom)
                self.comNumComboBox.setCurrentText(self.serialPort.portName()[3:])
                self.comNumComboBox.currentIndexChanged.connect(self.changeCurrCom)

            else:
                self.port_is_chosen = False
                self.renewAvailableComsList()

    def changeBitsNum(self):
        # print("Now " + self.bitsNumComboBox.currentText() + "is chosen")
        self.serialPort.close()
        self.serialPort.setDataBits(self.bitsNumComboBox.currentData())

        sleep(0.02)
        if self.serialPort.open(QIODeviceBase.OpenModeFlag.ReadWrite):
            self.debugTextBrowser.append(self.serialPort.portName() + " is successfully opened")
        else:
            QMessageBox.critical(None, "Error", self.serialPort.portName() + " cannot be reopened")
            self.port_is_chosen = False
            self.renewAvailableComsList()


if __name__ == "__main__":

    app = QApplication(sys.argv)
    ComsInfo_MW = ComInfoWindow()

    ComsInfo_MW.show()
    sys.exit(app.exec())