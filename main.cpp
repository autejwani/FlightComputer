#include <Arduino.h>
#include "Sensing/imu.h"
#include "Sensing/baro.h"
#include "Controls/ekf.h"

EKF ekf;
int baroCounter = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting...");

    if(!imu_init()) {
        Serial.println("IMU FAIL");
        while(1);
    }
    Serial.println("IMU OK");

    if(!baro_init()) {
        Serial.println("BARO FAIL");
        while(1);
    }
    Serial.println("BARO OK");

    float ground_alt;
    if(baro_read_altitude(ground_alt)) {
        ekf.init(ground_alt);
        Serial.print("Init: ");
        Serial.println(ground_alt);
    } else {
        ekf.init(0.0f);
        Serial.println("Init: 0");
    }

    Serial.println("Alt | Vel");
}

void loop() {
    float ax, ay, az;

    if(!imu_read_accel(ax, ay, az)) {
        Serial.println("READ FAIL");
        delay(10);
        return;
    }

    ekf.predict(ax, ay, az);

    baroCounter++;
    if(baroCounter >= 10) {
        baroCounter = 0;
        float altitude;
        if(baro_read_altitude(altitude)) {
            ekf.updateBaro(altitude);
        }

        Serial.print(ekf.getAltitude());
        Serial.print(" | ");
        Serial.println(ekf.getVelocity());
    }

    delay(10);
}
