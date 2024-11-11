import sys
from time import sleep

from PyQt6.QtSerialPort import QSerialPortInfo, QSerialPort
from PyQt6.QtWidgets import QMainWindow, QApplication, QMessageBox, QTextEdit
from PyQt6.QtCore import QObject, Qt, QIODevice, QByteArray, QTimer, QIODeviceBase
from serial import PortNotOpenError

from cominfo_window import Ui_ComsInterraction_MW
from PyQt6.QtGui import QIcon


class ComInfoWindow(QMainWindow, Ui_ComsInterraction_MW):
    def __init__(self):
        super(QMainWindow, self).__init__()
        self.setupUi(self)
        self.setWindowIcon(QIcon("port_i.jpg"))

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
            if self.serialPort.error() == QSerialPort.SerialPortError.NoError:
                bytes_written = self.serialPort.write(bytes(text, 'utf-8'))
                self.statusTextBrowser.append("Send " + str(bytes_written) + " bytes")
            else:
                QMessageBox.critical(None, "Error", "Cannot write data in " + self.serialPort.portName())
                self.port_is_chosen = False
                self.serialPort.close()
                self.renewAvailableComsList()

    def readDatas(self):
        data = self.serialPort.readAll()
        if self.serialPort.error() == QSerialPort.SerialPortError.NoError:
            text_data = str(data, 'utf-8')
            self.outputTextBrowser.append(text_data[:-1])
            self.statusTextBrowser.append("Received " + str(data.size()) + " bytes")
        else:
            QMessageBox.critical(None, "Error", "Cannot read data from " + self.serialPort.portName())
            self.port_is_chosen = False
            self.serialPort.close()
            self.renewAvailableComsList()

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



























































# global thread_is_gonna_end
# thread_is_gonna_end = True
# self.ser.close()
# time.sleep(1)
# self.receive_thread.join()

# if len(text) > self.previous_text_length:
#    if text.endswith('\n'):
#        #print('Символ "\\" обнаружен в конце текста.')
#        self.outputTextBrowser.setText(text)
# self.previous_text_length = len(text)

'''
self.op_port = QSerialPort("COM0")
self.op_port.setDataBits(QSerialPort.DataBits.Data8)
self.op_port.setBaudRate(9600)
self.op_port.setParity(QSerialPort.Parity.NoParity)
self.op_port.setStopBits(QSerialPort.StopBits.OneStop)
self.op_port.setFlowControl(QSerialPort.FlowControl.NoFlowControl)

test = self.op_port.open(QIODeviceBase.OpenModeFlag.ReadWrite)

self.op_port2 = QSerialPort("COM1")
self.op_port2.setDataBits(QSerialPort.DataBits.Data8)
self.op_port2.setBaudRate(9600)
self.op_port2.setParity(QSerialPort.Parity.NoParity)
self.op_port2.setStopBits(QSerialPort.StopBits.OneStop)
self.op_port2.setFlowControl(QSerialPort.FlowControl.NoFlowControl)

test = self.op_port2.open(QIODeviceBase.OpenModeFlag.ReadWrite)

self.op_port2.readyRead.connect(self.readDatttas)

data_to_send = b'Hello, World!222222222222222222\n'
self.op_port.write(data_to_send)
'''

'''

self.actionRenewComsNum.triggered.connect(self.iwannadie)

######################################################################

#'Port Datas:'


############## above is good ##################################################################


self.previous_text_length = 0
############## vrode good ##################################################################


self.port_name = "COM1"
self.baudrate = 9600
timeout = 1

self.ser = serial.Serial(self.port_name, baudrate=self.baudrate, timeout=timeout)

self.receive_thread = threading.Thread(target=self.receive_data)
self.receive_thread.start()

'''

'''

    def iwannadie(self):
        serro = serial.Serial('COM2', 9600)  # Имя порта и скорость (9600 бит/сек)

        data_to_send = b'Hello, World!\n'
        serro.write(data_to_send)  # Отправить данные, предварительно закодированные в байты

        serro.close()

        serro.open()  # Имя порта и скорость (9600 бит/сек)

        data_to_send = b'Hello, World!222222222222222222\n'
        serro.write(data_to_send)  # Отправить данные, предварительно закодированные в байты

        serro.close()

    def receive_data(self):
        try:
            while not thread_is_gonna_end:
                # Чтение данных из com-порта
                received_data = self.ser.readline()
                str = received_data.decode('utf-8')
                if len(str):
                    print("Прочитанные данные:", str)
                self.statusTextBrowser.append(str)
                # Обработка данных
                # ...
        except PortNotOpenError as e:
            print("Просто не открылся порт")
        except Exception as e:
            print(f"Ошибка: {e}")

        finally:
            print("End of thread")

'''

# string = self.serialPort.portName()
# print("current com:" + self.serialPort.portName())
# print("self.serial port:" + curr_com.portName())
# print("Trying to change port on COM " + self.comNumComboBox.currentText())
# print("::" + "COM" + self.comNumComboBox.currentText())
# print(self.serialPort.portName() + " is successfully opened")
# print(self.serialPort.portName() + " cannot be opened")


# bytes_data = bytes(data.data())
# receive = bytes_data.decode('cp1251')
# print("Прочитано:", text_data)
# print("Прочитано bytes:", receive)

# receive = self.s_port.read().decode('cp1251')
# self.outputTextBrowser.append(text_data[:-1]) # receive = self.s_port.read().decode('cp1251')