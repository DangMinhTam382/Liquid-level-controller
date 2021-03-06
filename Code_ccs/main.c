#include <main.h>

//#include "delay.c"

#include "def_16f877a.h"
#FUSES NOWDT, HS, NOPUT, NOPROTECT, NODEBUG, NOBROWNOUT,NOLVP, NOCPD, NOWRT

#define LCD_ENABLE_PIN  PIN_D3                                    ////
#define LCD_RS_PIN      PIN_D1                                    ////
#define LCD_RW_PIN      PIN_D2                                    ////
#define LCD_DATA4       PIN_D4                                    ////
#define LCD_DATA5       PIN_D5                                    ////
#define LCD_DATA6       PIN_D6                                    ////
#define LCD_DATA7       PIN_D7  
#include <lcd.c>
// 

// PORTC => 8 BUTTONS
#define START        RC0
#define STOP         RC1
#define SEL          RC2
#define CHANGE       RC3
#define INC50        RC4
#define DEC50        RC5
#define INC10        RC6
#define DEC10        RC7

// Khai bao Chan Relay:
#define RELAY_UPPER     PIN_B2
#define RELAY_BELOW     PIN_B1


//-----------------------------//
//Khai bao Mode hoat dong:
char water[] = "NUOC  ";
char gasoline[] = "XANG  ";
char alcohol[] = "RUOU  ";
char milk[] = "SUA   ";
char cc_oil[] = "D_AN  ";
//-----------------------------//
int8 const increasehigh = 10;
int8 const increaselow = 1;
int8 const decreasehigh = 10;
int8 const decreaselow = 1;
int8 count_mode = 1;

float temp = 0;
float Volume = 0;
float high = 0;
float trongluongrieng = 1;
long setval = 0; //gia tri chieu cao cai dat
int1 start_stop_check = 0;
int1 upper_on = 0, below_on = 0;

void main()
{
   //hangtram = setval div 100;
   
   //VALVE1 = 1;
   TRISC=0xFF;
   output_low (RELAY_UPPER);
   output_low (RELAY_BELOW);
   
   setup_adc_ports(0);
   setup_adc(ADC_CLOCK_DIV_2);
   float adc = 0, voltage = 0, weight = 0;
   
//   hien thi cac dong chu tren lcd
   lcd_init();
   lcd_gotoxy(11,1);
   char realheight[] = "Real_H";
   for (int i=0; i < 6; i++)  lcd_putc(realheight[i]);
   lcd_gotoxy(10,1);
   lcd_putc('|');
   lcd_gotoxy(10,2);
   lcd_putc('|');
   lcd_gotoxy(1,1);
   char modeselect[] = "M:";
   for (int j=0; j < 2; j++)  lcd_putc(modeselect[j]);
   char height[] = "H_SET:";
   lcd_gotoxy(1,2);
   for (int k=0; k < 6; k++)  lcd_putc(height[k]);
      
// mac dinh la nuoc, trong luong rieng = 1 
   char mode[]="NUOC  ";
   lcd_gotoxy(3,1);
   for (int m=0; m < 6; m++)  lcd_putc(mode[m]);
//

   while(TRUE)
   {
      //Doc ADC va tinh chieu cao muc nuoc:
      adc = read_adc();
      voltage = adc*5/65535;
      weight = adc * 3500/65535;
      Volume = weight / trongluongrieng;
      high = weight /(trongluongrieng*10);

      lcd_gotoxy (11,2);
      printf(lcd_putc, "%3.4f", high);
      
      //Nhan nut Start:
      if (START == 0)   start_stop_check = 1;
      
      //Nhan nut STOP:     
      else if (STOP == 0) {
         output_low (RELAY_BELOW);
         output_low (RELAY_UPPER);
         start_stop_check = 0;
      }
      //Stop == 0; Start == 1;
      if (start_stop_check == 0) {
          
          //Nhan nut CHANGE:
          if (CHANGE == 0) {
             if (count_mode == 5)   count_mode = 1;
             else count_mode += 1;
             
             //Hien thi Mode:
             lcd_gotoxy(3,1);
             if (count_mode == 1)      {for (int i=0; i < 6; i++)  lcd_putc(water[i]);}
             else if (count_mode == 2) {for (int i=0; i < 6; i++)  lcd_putc(gasoline[i]);}
             else if (count_mode == 3) {for (int i=0; i < 6; i++)  lcd_putc(alcohol[i]);}
             else if (count_mode == 4) {for (int i=0; i < 6; i++)  lcd_putc(milk[i]);}
             else                      {for (int i=0; i < 6; i++)  lcd_putc(cc_oil[i]);}
          }
          //Nhan nut SEL:
          else if (SEL == 0) {
             //Chon khoi luong rieng:
             if (count_mode == 1)      trongluongrieng = 1;
             else if (count_mode == 2) trongluongrieng = 0.7;
             else if (count_mode == 3) trongluongrieng = 0.79;
             else if (count_mode == 4) trongluongrieng = 1.03;
             else                      trongluongrieng = 0.9;
          }
          
          //Nhan nut chinh chieu cao:
          if      (setval<(301-increasehigh) && INC50 == 0)  setval += increasehigh;
          else if (setval>(decreasehigh-1)   && DEC50 == 0)  setval -= decreasehigh;
          else if (setval<(301-increaselow)  && INC10 == 0)  setval += increaselow;
          else if (setval>(decreaselow-1)    && DEC10 == 0)  setval -= decreaselow;
          

          //Hien thi gia tri cai dat len lcd:
          lcd_gotoxy (7,2);
          printf(lcd_putc, "%3.0ld", setval);
          temp = setval-high;
          delay_ms (200);
      }   //start_stop_check
      
      // Nhan nut Start: sau chinh 
      else {
         if (temp > 0) {
            output_high (RELAY_UPPER);
            upper_on = 1;
         }
         else if (temp <0) {
            output_high (RELAY_BELOW);
            below_on = 1;
         }
         
         //Dung Relay:
         if (upper_on && high >= setval) {
            output_low (RELAY_UPPER);
            upper_on = 0;
            temp = 0;
         }
         else if (below_on && high <= setval) {
            output_low (RELAY_BELOW);
            below_on = 0;
            temp = 0;
         }   
      }//Star == 1
      
   }//while true
}// Main

