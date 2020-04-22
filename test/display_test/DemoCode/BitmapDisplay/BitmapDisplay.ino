#include <Wire.h>
#include <I2C_LCD.h>

I2C_LCD LCD;
extern GUI_Bitmap_t Tuzki_1;   //Declare bitmap data packet.
uint8_t I2C_LCD_ADDRESS = 0x51;  //Device address setting, default: 0x51

void setup(void)
{
    Wire.begin();        //I2C controler init.
}

void loop(void)
{
    LCD.CleanAll(WHITE);    //Erase all.
    delay(1000);            //Delay for 1s. 

    //Booting logo ON, backlight ON, bitmap work mode.
    //If you want to display characters please switch to WM_CharMode.
    LCD.WorkingModeConf(ON, ON, WM_BitmapMode);
    
    //Display bitmap at the specified location.
    LCD.DrawScreenAreaAt(&Tuzki_1, 30, 0);
    
    while(1); //Wait for ever.
}
