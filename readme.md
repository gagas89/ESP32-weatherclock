# ESP32-weatherclock

ESP32 microcontroller based desktop weather clock that show you time, date, current weather and weather forecasting using 2.4inch ILI9341 tft LCD display.



##### Material Used

1. ###### ESP32 Development board

&nbsp;	In this project I'm using UNO compatible ESP32-S3 board. yes, this ESP32 development board looks the same with Arduino UNO. I love this board, thus I could use any UNO or mega compatible shield available on the market.


###### 2.4inch TFT\_LCD with ILI9341

&nbsp;	I'm using 2.4inch tft LCD with ILI 3941 driver that UNO shield shaped. however, not all native pin are available for driving this display. so I put jumper connecting GPIO46 to GPIO15 on the UNO shaped ESP32 board. This way, I 	can use the tft LCD as a shield. Here's how I connect the LCD to the ESP32. 



// The ESP32 and TFT the pins used for testing are:

\#define TFT\_CS   6  // Chip select control pin (library pulls permanently low

\#define TFT\_DC   7  // Data Command control pin - must use a pin in the range 0-31

\#define TFT\_RST  5  // Reset pin, toggles on startup



\#define TFT\_WR    1  // Write strobe control pin - must use a pin in the range 0-31

\#define TFT\_RD    2  // Read strobe control pin



\#define TFT\_D0   21  // Must use pins in the range 0-31 for the data bus

\#define TFT\_D1   15  // so a single register write sets/clears all bits.

\#define TFT\_D2   18  // Pins can be randomly assigned, this does not affect

\#define TFT\_D3   17  // TFT screen update performance.

\#define TFT\_D4   19

\#define TFT\_D5   20

\#define TFT\_D6   03

\#define TFT\_D7   14



Anyway, with Shield shaped LCD and Uno Shaped ESP32, I simply sandwiched the board and jumper connect the IO46 and IO15.



##### Arduino IDE

In this repository, I'm using Arduino IDE including the library management



##### Library Used

###### &nbsp;	1. PNGdec by larry bank 

###### &nbsp;	2. TFT\\\_eSPI by bodmer 

&nbsp;		If you are familiar with the tft\_eSPI library, you're good to go. if you're first using this library, make sure you read the readme file and do all necessary things. here I'm using parallel ILI9341, the USER\_SETTING.h 		file included here in this repo meant to work with my setup.

###### &nbsp;	3. arduinojson by benoit 

ith those libraries, I'm afraid you are running out of memory somehow. So consider to expand your flash memory in Arduino IDE Setup.



##### API 

1. ###### NTP Server

&nbsp;	You will have no problem with NTP server, there's no token needed. you just need to make sure that the ntp servers are valid and active.



###### 2\. Weather Information @ https://openweathermap.org/

&nbsp;	Create an account on this website and get your token for the API accees. then put your token on "apiKey" string definition in === Weather API === section in the Arduino sketch.

&nbsp;	You can also define your location, just make sure the location you gonna use is registered in the openweathermap.org listing. change your location in "city" string. 

