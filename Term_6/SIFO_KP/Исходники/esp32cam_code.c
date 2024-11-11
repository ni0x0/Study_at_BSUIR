#include <Arduino.h>

// MicroSD Libraries
#include "FS.h"
#include "SD_MMC.h"

// Camera libraries
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
 
// Pin definitions for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void initMicroSDCard() {
  // Start the MicroSD card
 
  //Serial.println("Mounting MicroSD Card");
  if (!SD_MMC.begin()) {
    //Serial.println("MicroSD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    //Serial.println("No MicroSD Card found");
    return;
  }
}


void configESPCamera() {
  // Configure Camera parameters
 
  // Object to store the camera configuration parameters
  camera_config_t config;
 
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  config.photo_take = LAST_IN_BUFF; 
 
  // Select lower framesize if the camera doesn't support PSRAM
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
 
  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
 
  // Camera quality adjustments
  sensor_t * s = esp_camera_sensor_get();
 
  s->set_contrast(s, 2);
  s->set_brightness(s, 2);
  s->set_saturation(s, 2);
  s->set_gainceiling(s, GAINCEILING_16X);
  s->set_colorbar(s, 0);
  s->set_whitebal(s, 0);
  s->set_hmirror(s, 0);
  s->set_vflip(s, 0);
  s->set_ae_level(s, 0);
  s->set_special_effect(s, 0);
  s->set_wb_mode(s, 2);
  s->set_awb_gain(s, 1);
  s->set_bpc(s, 1);
  s->set_wpc(s, 1);
  s->set_raw_gma(s, 1);
  s->set_lenc(s, 0);
  s->set_agc_gain(s, 1);
  s->set_aec_value(s, 600);
  s->set_gain_ctrl(s, 0);
  s->set_exposure_ctrl(s, 1);
  s->set_aec2(s, 1);
  s->set_dcw(s, 0);
 
}


void takeNewPhoto(String path) {
  // Take Picture with Camera
 
  // Setup frame buffer
  camera_fb_t * fb = esp_camera_fb_get();
 
  if (!fb) {
    //Serial.println("Camera capture failed");
    return;
  }
 
  // Save picture to microSD card
  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    //Serial.println("Failed to open file in write mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    //Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  // Close the file
  file.close();
 
  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}


void setup() {

  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Start Serial Monitor
  Serial.begin(9600);

  // Initialize the camera
  //Serial.print("Initializing the camera module...");
  configESPCamera();
  //Serial.println("Camera OK!");

  // Initialize the MicroSD
  //Serial.print("Initializing the MicroSD card module... ");
  initMicroSDCard();
}
 
void loop() {

  if(Serial.available() > 0) {
    String income_file_name = Serial.readStringUntil('\n');
    income_file_name = income_file_name.substring(0, income_file_name.length() - 1);
    income_file_name = "/" + income_file_name;
    delay(100);
    takeNewPhoto(income_file_name);
  }
  delay(100);
}

