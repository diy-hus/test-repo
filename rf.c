#define CE PORTC.0      //23
#define CSN PORTC.1     //24
#define SCK PORTC.2     //25
#define MOSI PORTC.3    //26
#define MISO PINC.4     //27
#define IRQ PINC.5      //28
//--------------*---------------
unsigned char SPI_RW(unsigned char Buff);                                       //Function used for text moving
unsigned char SPI_Read(void);
void RF_Init();                                                                 //Function allow to Initialize RF device
void RF_Write(unsigned char Reg_Add, unsigned char Value);                      //Function to write a value to a register address
void RF_Command(unsigned char command);                                         //Function to write a command
void RF_Write_Address(unsigned char Address);                                   //Function to write TX and RX address
void RX_Mode();                                                          //Function to put nRF in RX mode
void RF_Config();                                                               //Function to config the nRF
unsigned char RF_RX_Read();                                                     //Function to read the data from RX FIFO

unsigned char SPI_RW(unsigned char Buff)
{
    unsigned char bit_ctr;
       for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
       {
        MOSI = (Buff & 0x80);         // output 'uchar', MSB to MOSI    
        delay_us(5);
        Buff = (Buff << 1);           // shift next bit into MSB..
        SCK = 1;                      // Set SCK high..          
        delay_us(5);
        Buff |= MISO;                 // capture current MISO bit
        SCK = 0;                      // ..then set SCK low again
       }
    return(Buff);                     // return read uchar
}
unsigned char SPI_Read(void)
{   unsigned char Buff=0;
    unsigned char bit_ctr;
       for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
       {
        delay_us(5);
        Buff = (Buff << 1);           // shift next bit into MSB..
        SCK = 1;                      // Set SCK high..          
        delay_us(5);
        Buff |= MISO;                 // capture current MISO bit
        SCK = 0;                      // ..then set SCK low again
       }
    return(Buff);                     // return read uchar
}
void RF_Init()                                                    //Function allow to Initialize RF device
{ 
    CE=1;
    delay_us(700);
    CE=0;
    CSN=1;
}
void RF_Write(unsigned char Reg_Add, unsigned char Value)         //Function to write a value to a register address
{
    CSN=0;
    SPI_RW(0b00100000|Reg_Add);
    SPI_RW(Value);
    CSN=1;
    delay_us(10);
}
void RF_Command(unsigned char command)                            //Function to write a command
{
    CSN=0;
    SPI_RW(command);
    CSN=1;
    delay_us(10);
}
void RF_Write_Address(unsigned char Address)                      //Function to write TX and RX address
{
    CSN=0;
    RF_Write(0x03,0b00000011);
    CSN=1;
    delay_us(10);
    CSN=0;
    SPI_RW(0b00100000|0x0A);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    CSN=1;
    delay_us(10);
    CSN=0;
    SPI_RW(0b00100000|0x10);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    SPI_RW(Address);
    CSN=1;
    delay_us(10);
}

void RX_Mode()                                             //Function to put nRF in RX mode
{
    RF_Write(0x00,0b00011111);     //CONFIG 0x00
    CE=1;
}

void RF_Config()                                                  //Function to config the nRF
{
RF_Command(0b11100010);     //Flush RX
delay_us(10);
RF_Write(0x00,0b00011111);     //CONFIG 0x00
delay_ms(2);
RF_Write(0x07,0b01111110);
RF_Write(0x11,0b00000001);     //RX_PW_P0 0x11     Payload size
RF_Write(0x05,0b00000010);     //RF_CH 0x05        Choose frequency channel
RF_Write_Address(Code_tay_cam);
RF_Write(0x02,0b00000001);     //EX_RXADDR 0x02    enable data pipe 0;
RF_Write(0x01,0b00000001);     //EN_AA 0x01        enable auto-acknowledgment
RF_Write(0x04,0b00000000);     //SETUP_RETR 0x04   Setup retry time
}
unsigned char RF_RX_Read()                                         //Function to read the data from RX FIFO
{
   unsigned char dem_RX;
   CE=0;
   CSN=1;
   delay_us(10);
   CSN=0;
   SPI_RW(0b01100001);
   delay_us(10);
   dem_RX = SPI_Read();
   CSN=1;
   CE=1;
   RF_Write(0x07,0b01111110);  // Clear flag
   RF_Command(0b11100010);     //Flush RX
   return dem_RX;
}