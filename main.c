
#include "mcc_generated_files/mcc.h"


uint16_t convertEchoToCm(uint16_t timerCounts);
uint16_t echoPulse = 0;
uint8_t newSample = 1;

#define NUM_SAMPLES 8
#define ANGLES 19
#define CW 400
#define CCW 1700
#define RANGING 4
#define NUM_ROWS 16
#define NUM_COLUMNS 34

    uint8_t    map[50][50];

//----------------------------------------------
// Main "function"
//----------------------------------------------
void milliSecondDelay(uint16_t ms);
void microSecondDelay(uint16_t us);
void main (void) {
    
    TRISA = 0;
    SIGNAL_PIN_SetLow();
    
    uint16_t    distance[ANGLES];
    uint16_t    sample[NUM_SAMPLES];
    uint16_t    left[NUM_SAMPLES];
    uint16_t    neutral[NUM_SAMPLES];
    uint16_t    right[NUM_SAMPLES];
    uint16_t    angle[ANGLES];
    uint8_t     i;
    uint8_t     degree;
    int8_t     xPoints[ANGLES];
    int8_t     yPoints[ANGLES];


    //              0
    uint16_t anglesUs[ANGLES] = {400,472,544,617,689,761,833,906,978,1050,1122,1194,1267,1339,1411,1484,1556,1628,1700};
    //                            0   10  20  30  40  50  60  70  80  90   100  110  120  130  140  150  160  170  180
    int16_t cosValues[ANGLES] = {128,126,120,111,98, 82, 64, 44, 22,  0, -22,  -44,  -64, -82,-98,-111,-120,-126,-128};
    uint16_t sinValues[ANGLES] = {0 , 22, 44, 64, 82, 98, 111, 120, 126, 128, 126, 120, 111, 98, 82, 64, 44, 22, 0};

    char        cmd;
            
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
        
	printf("Dev'17 Board\r\n");
	printf("inLab 05 terminal\r\n");
    printf("Ultrasonic Connections:\r\n");
    printf("    Vcc: left pin on JP1 header (closest to mini USB).\r\n");
    printf("    Trig:   RC7\r\n");
    printf("    Echo:   RB0\r\n");
    printf("    GND:    GND header pin on PIC breakout header.\r\n");
    printf("\r\n");
    printf("Servo Motor Connections:\r\n");
    printf("Black wire to GND pin on right side of breakout pins.\r\n");
    printf("Red wire:    RA5\r\n");
    printf("White wire:  RA3\r\n");
	printf("\r\n\n\n\n> ");                       // print a nice command prompt

	for(;;) {

		if (EUSART2_DataReady) {			// wait for incoming data on USART
            cmd = EUSART2_Read();
			switch (cmd) {		// and do what it tells you to do

                case '?':
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");                   
                    printf("o: k\r\n");
                    printf("R: Reset processor\r\n");  
                    printf("z: Clear terminal\r\n");
                    printf("s: Stream Echo pulse length\r\n");  
                    printf("S: Stream distance\r\n");             
                    printf("P: Print Current (x,y) Coordinate Values\r\n");
                    printf("J: Rotate Left\r\n");
                    printf("K: Set Neutral\r\n");
                    printf("L: Rotate Right\r\n");
                    printf("c: Rotate Clockwise\r\n");
                    printf("C: Rotate CounterClockwise\r\n");
                    printf("f: Full rotation and sample gathering\r\n");
                    printf("g: Print out environment to terminal\r\n");
                    printf("------------------------------\r\n");                    
                    break;
                    
                //--------------------------------------------
                // Reply with "k", used for PC to PIC test
                //--------------------------------------------    
                case 'o':
                    printf(" k\r\n");
                    break;

                //--------------------------------------------
                // Reset the processor - be careful with this one
                //--------------------------------------------                      
                case 'R':
                    RESET();                    
                    break;
                    
                case 'z':
                    for(i = 0; i < 40; i++) printf("\n");
                    break;
    
                //--------------------------------------------
                // Stream echo pulse duration in timer counts
                //--------------------------------------------                      
                case 's':
                    printf("press any key to exit\r\n");
                    while(!EUSART2_DataReady) {                        
                        printf("%8u\r",echoPulse);
                    }
                    (void) EUSART2_Read();
                    printf("\r\n");
                    break;

                //--------------------------------------------
                // Stream echo pulse duration in distance
                //--------------------------------------------                      
                case 'S':
                    printf("press any key to exit\r\n");
                    while(!EUSART2_DataReady) {                        
                        printf("%8u\r",convertEchoToCm(echoPulse));
                    }
                    (void) EUSART2_Read();
                    printf("\r\n");
                    break;                    
                    
                //Print out (x,y) coordinates of gathered environment
                case 'P':
                
                    for(uint8_t i=0;i<19;i++) {
                        printf("(%d, %d)\r\n",xPoints[i],yPoints[i]);
                    }
                   
                    break;

                case 'L':
                    printf("Rotate right\r\n");
                    uint8_t i;
                    for(i=0;i<50;i++) {
                        SIGNAL_PIN_SetHigh();
                        __delay_us(1000);
                        SIGNAL_PIN_SetLow();
                        __delay_us(18500);
                    }
                    printf("press any key to exit\r\n");
                    
                    for (i=0; i<NUM_SAMPLES; i++) {
                        while (newSample == 0);
                        left[i] = echoPulse;
                        newSample = 0;
                    }
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",left[i]);
                    printf("\r\n");
                    
                    uint8_t j = 1;
                    for(i = 0; i < NUM_SAMPLES - 1; i++){
                        for(j = 1; j < NUM_SAMPLES - i - 1; j++){
                            if(left[j]>left[j+1]){
                                uint16_t temp = left[j];
                                left[j] = left[i+1];
                                left[i+1] = temp;
                            }
                            
                        }
                    }
                    
                    
                    angle[0] = 0;
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",left[i]);
                    
                    printf("\r\n");
                    break;
                    
                case 'K':
                    
                    printf("Set neutral\r\n");
                    uint8_t i;
                    for(i=0;i<50;i++) {
                        SIGNAL_PIN_SetHigh();
                        __delay_us(1500);
                        SIGNAL_PIN_SetLow();
                        __delay_us(19200);
                    }
                    printf("press any key to exit\r\n");
                    
                    for (i=0; i<NUM_SAMPLES; i++) {
                        while (newSample == 0);
                        neutral[i] = echoPulse;
                        newSample = 0;
                    }
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",neutral[i]);
                    printf("\r\n");
                    
                    uint8_t j = 1;
                    for(i = 0; i < NUM_SAMPLES - 1; i++){
                        for(j = 1; j < NUM_SAMPLES - i - 1; j++){
                            if(neutral[j]>neutral[j+1]){
                                uint16_t temp = neutral[j];
                                neutral[j] = neutral[i+1];
                                neutral[i+1] = temp;
                            }
                            
                        }
                    }
                    
                    angle[1] = 45;
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",neutral[i]);
                    printf("\r\n");
                   break;
                case 'J':
                    printf("Rotate left\r\n");
                    uint8_t i;
                    for(i=0;i<50;i++) {
                        SIGNAL_PIN_SetHigh();
                        __delay_us(2500);
                        SIGNAL_PIN_SetLow();
                        __delay_us(17800);
                    }           
                    for (i=0; i<NUM_SAMPLES; i++) {
                        while (newSample == 0);
                        right[i] = echoPulse;
                        newSample = 0;
                    }
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",right[i]);
                    printf("\r\n");
                    
                    uint8_t j = 1;
                    for(i = 0; i < NUM_SAMPLES - 1; i++){
                        for(j = 1; j < NUM_SAMPLES - i - 1; j++){
                            if(right[j]>right[j+1]){
                                uint16_t temp = right[j];
                                right[j] = right[i+1];
                                right[i+1] = temp;
                            }
                            
                        }
                    }
                    
                    angle[2] = 90;
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",right[i]);
                    printf("\r\n");
                  break;  
                case '1':
                  SIGNAL_PIN_SetHigh();
                  __delay_us(CW);
                  SIGNAL_PIN_SetLow();
                  __delay_ms(20);
                  break;
                case '2':
                  SIGNAL_PIN_SetHigh();
                  __delay_us(CCW);
                  SIGNAL_PIN_SetLow();
                  __delay_ms(20);
                  break;
                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
                case 'f':
                    for(uint8_t x=0;x<50;x++) {
                        SIGNAL_PIN_SetHigh();
                        __delay_us(CW);
                        SIGNAL_PIN_SetLow();
                        __delay_ms(20);
                    }
   //Resets motor position to zero degrees
                    degree=0;
                    i=0;
                    for(uint16_t x=0;x<19;x++) {
                        angle[i] = degree;
                    SIGNAL_PIN_SetHigh();
 
                    microSecondDelay(anglesUs[x]);
                    SIGNAL_PIN_SetLow();
                    
                    __delay_ms(200); 
                    degree+=10;
                    i++;
                    }
                    __delay_ms(1000);
                    
                    i=18;
                    degree=180;
                    for(uint16_t x=18;x>0;x= x-1) {
                        angle[i] = degree;
                     SIGNAL_PIN_SetHigh();
                     microSecondDelay(anglesUs[x]);
                     SIGNAL_PIN_SetLow();
                     __delay_ms(200);   
                     i--;
                     degree = degree-10;
                    }
                    
                    for(uint8_t i=0;i<19;i++) {
                        uint16_t range = convertEchoToCm(echoPulse);
                        uint16_t rangeScaled = range >> 2;
                        int16_t sinVal = (convertEchoToCm(echoPulse) * sinValues[i]);
                        int16_t cosVal = (convertEchoToCm(echoPulse) * cosValues[i]);
                        int16_t xDir1 = cosVal >> 2;
                        int16_t yDir1 = sinVal >> 2;
                        int16_t xDir = xDir1 >> 7;
                        int16_t yDir = yDir1 >> 7;
                        printf("Angle:       %d\r\n", angle[i]);
                        printf("Range:       %d\r\n", range);
                        printf("Range Scaled:%d\r\n", rangeScaled);
                        printf("RangeCos:    %d\r\n", xDir);
                        printf("RangeSin:    %d\r\n", yDir);
                        xPoints[i] = (xDir*2)+40;
                        yPoints[i] = (yDir*2)+1;
                        //printf("(%d, %d)\r\n", xPoints[i],yPoints[i]);
  
                        
                        
                    }
                    break;
                case 'g':
                    for(i=0;i<30;i++) {
                        for(uint8_t j=0;j<80;j++) {
                              
                            for(uint8_t z = 0;z<20;z++) {
                                if(xPoints[z] == j) {
                                    if(yPoints[z] == i) {
                                        printf("*");
                                    }
                                }
                            }
                            
                            
                            
                            if(i == 0 || i == 29) {
                                printf("-");
                            } else {
                                if( (j == 0 || j == 79) && (i != 0 || i!=29)) {
                                printf("|");
                                
                            } else{
                                printf(" ");
                            } 
                            }
                            
                            
                          
                        }
                        printf("\r\n");
                    }
                  
                default:
                    printf("Unknown key %c\r\n",cmd);
                    break;
			} // end switch
            
		}	// end if
	} // end while 
} // end main

//------------------------------------------------------------------------
// This routine uses the Capture module inside the CCP4 unit to look for
// a positive edge of the echo pulse on RB0.  When this happens, the 
// ECCP1_CaptureRead function return the timer 1 value when this happened.  
// The code then  configures itself to look for a falling edge of the 
// the RB0 pin.  When the negative edge occurs, the CCP module captures
// the timer 1 value in the CCPR1 register.  The subsequent call to
// ECCP1_CaptureRead returns this value into the end variable.  The difference
// between end and start is returned as the proxy for the distance.
//
// You will need to convert this function into the CCP4_CallBack ISR.  Under 
// no circumstance are you to leave a bus wait look like:
//          while(CCP4_IsCapturedDataReady() == false);
// in your ISR code.  Since the ISR is called on a negative or positive edge, 
// there is no reason to have the ISR wait, just have the ISR exit.  The ISR 
// will then be called when the edge occurs.  
//------------------------------------------------------------------------


//------------------------------------------------------------------------
//------------------------------------------------------------------------

void CCP4_CallBack(uint16_t capturedValue) {
    static uint16_t start = 0;
    static uint16_t end = 0;
    
    if(CCP4CONbits.CCP4M == 0b0101) {
        start = capturedValue;
        CCP4CONbits.CCP4M = 0b0100;
    }
    else{
        
        end = capturedValue;
        CCP4CONbits.CCP4M = 0b0101;
        echoPulse = end-start;
        newSample = 1;
    }
    return; 
}


//------------------------------------------------------------------------
// Hold the trigger high for 25us, once every 100ms
//------------------------------------------------------------------------
void TMR0_DefaultInterruptHandler(void){

    uint16_t i;
    
    TRIG_PIN_SetHigh();	
    for (i=0; i<32; i++);      // Busy wait 25uS for trigger
    TRIG_PIN_SetLow();	    

} // end 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
uint16_t convertEchoToCm(uint16_t timerCounts) {
    return(547* ((uint32_t) timerCounts) >> 16);
}
void milliSecondDelay(uint16_t ms) {
    uint16_t i;
    for(i=0; i< ms; i++) {
        microSecondDelay(1000);
    }
}
void microSecondDelay(uint16_t us) {
    uint16_t i;
    for(int i=0;i<us;i++) {
       #asm
       NOP //1
       NOP //2
       NOP //3
       NOP //4
       NOP //5
       NOP //6
       #endasm
       i = i;
    }
} 
