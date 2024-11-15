This project is a simple yet effective solution to combat noise pollution. It's designed to monitor ambient noise levels and trigger alerts when the noise level crosses a predefined threshold.

Hardware Components:

1.ESP8266 Wi-Fi Module
2.16x2 LCD
3.Microphone Sensor
4.5V Buzzer
5.I2C Module


Software requirements:

1.Blynk
2.Twilio
3.Arduino IDE

Working:
*Noise Detection: A microphone sensor captures the ambient noise.
*Data Processing: The ESP8266 Wi-Fi module processes the analog noise signal into digital data.
*Threshold Comparison: The processed data is compared to a set noise threshold.
*Alert Activation: If the noise level exceeds the threshold:
*An SMS alert is sent via Twilio.
*A visual alert is displayed on the 16x2 LCD.
*An audible alert is triggered by the buzzer.
*Remote Monitoring: Real-time noise levels are monitored on the Blynk platform.

NOTE: the SHA1Fingerprint changes every year, so visit (https://api.twilio.com/) to view the fingerprint, the connection will not be esablishes unless the fingerprint matches.

referneces:
https://www.youtube.com/watch?v=SjT_yLsVGiI
https://www.instructables.com/Send-an-SMS-Using-an-ESP8266/
