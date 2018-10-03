/*
 * Display a test card of the SSD1306 display
 */
#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif

#include <avr/io.h>
#include <util/delay.h>                // for _delay_ms()

#define SSD1306_128_64
#include "ssd1306.h"
#include "sprites.h"

/*
 * CS = PB0  (SS)
 * DC = PB1  (Data/Command)
 * RES = PB2 (Reset)
 * D1 = PB3 (MOSI)
 * D0 = PB5 (SCK)
 *
 */
#define CS_PIN PORTB0
#define DC_PIN PORTB1
#define RES_PIN PORTB2
#define MOSI_PIN PORTB3
#define SCK_PIN PORTB5

void spi_init (void);
void ssd1306_command(uint8_t c);
void display(void);

/*
 * Macro to write one byte to the SPI bus and wait for transmission to complete.
 */
#define WRITE(b) { SPDR = (b); while(!(SPSR & (1<<SPIF) )); }

#define PIN_HIGH(port, pin) ((port) |= (1<<pin))
#define PIN_LOW(port, pin) ((port) &= ~(1<<pin))
// Euphemisms that are more natural when setting register bits
#define SET_BIT(port, pin) ((port) |= (1<<pin))
#define CLEAR_BIT(port, pin) ((port) &= ~(1<<pin))

int main(void)
{
  spi_init();

  // We're not providing an externally regulated 7V supply to drive the screen.
  // Instead, the SSD1306 has an inbuilt boost regulator which provides 7V Vcc
  // out of the 5V Vdd.
  int vccstate = SSD1306_SWITCHCAPVCC; // SSD1306_EXTERNALVCC;

  // Bring out of reset
  PIN_LOW(PORTB, RES_PIN);
  _delay_ms(10);
  PIN_HIGH(PORTB, RES_PIN);

  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE

  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(SSD1306_LCDHEIGHT - 1);

  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D

  if (vccstate == SSD1306_EXTERNALVCC)
  {
    ssd1306_command(0x10);
  }
  else
  {
    ssd1306_command(0x14);
  }


  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);

  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    ssd1306_command(0x9F);
  }
  else
  {
    ssd1306_command(0xCF);
  }

  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    ssd1306_command(0x22);
  }
  else
  {
    ssd1306_command(0xF1);
  }
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

  display();

  /*
   * Drop into 'blinky'
   */
  SET_BIT(DDRC, DDC5);             // Arduino pin A5 as output (PC5)
  while(1)
  {
      // LED on
      PIN_HIGH(PORTC, PORTC5);       // PC0 = High = Vcc
      _delay_ms(500);                // wait 500 milliseconds

      //LED off
      PIN_LOW(PORTC, PORTC5);        // PC0 = Low = 0v
      _delay_ms(500);                // wait 500 milliseconds
  }
}


// Initialize SPI Slave Device
void spi_init (void)
{

  // Set MOSI, SCK, CS, DC & RES as output
  DDRB = (1<<MOSI_PIN) | (1<<SCK_PIN) | (1<<CS_PIN) | (1<<DC_PIN) | (1<<RES_PIN);

  // We want MSB_FIRST endianess and SPI_MODE0
  // - Not including DORD gives MSB first
  // - Not including CPOL mean SCK is low at ldle
  // - Not including CPHA means sample on leading edge
  // -- In other words, SPI mode 0
  // Remove SPR0 to devide clock by 4, instead of 16.
  SPCR = (1<<SPE)|(1<<MSTR); // |(1<<SPR0);
}

/*
 * Adpated from Adafruit SSD1306 driver.
 */
void ssd1306_command(uint8_t cmd)
{
  // Make sure DC is low on falling edge of CS to select 'command'.
  PIN_HIGH(PORTB, CS_PIN);
  PIN_LOW(PORTB, DC_PIN);
  PIN_LOW(PORTB, CS_PIN);

  WRITE(cmd);

  // Set CS high to end command.
  PIN_HIGH(PORTB, CS_PIN);
}


void display(void)
{
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  #if SSD1306_LCDHEIGHT == 64
    ssd1306_command(7); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 32
    ssd1306_command(3); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 16
    ssd1306_command(1); // Page end address
  #endif

  // SPI
  PIN_HIGH(PORTB, CS_PIN);
  PIN_HIGH(PORTB, DC_PIN);
  PIN_LOW(PORTB, CS_PIN);

  // Write cat sprite
  for (int offset = 0; offset < 1024; ++offset)
  {
    WRITE(cat_128_64[offset]);
  }

  PIN_HIGH(PORTB, CS_PIN);
}
