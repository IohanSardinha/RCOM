#define FLAG	 0x7e
#define C_SET  	 0x03
#define C_UA	 0x07
#define C_DISC 	 0x0b
#define A_EM 	 0x03
#define A_RC 	 0x01

char SET[] = {FLAG, A_EM, C_SET, 0x00, FLAG}; 
char UA[]  = {FLAG, A_EM, C_UA , 0x04, FLAG};


 