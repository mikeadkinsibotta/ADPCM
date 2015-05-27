/*****************************************************************************
*	Filename: pcadpcm.h                                                      *
******************************************************************************
*	Author:  Rodger Richey                                                   *
*	Title:  Senior Applications Engineer                                     *
*	Company: Microchip Technology Incorporated                               *
*	Revision: 0                                                              *
*	Date: 1-11-96                                                            *
*	Compiled using Borland C+ Version 3.1                                    *
******************************************************************************
*	This is the header file that contains the ADPCM structure definition     *
*	and the function prototypes.                                             *
*****************************************************************************/
struct ADPCMstate {
	signed short prevsample;/* Predicted sample */
	int previndex;/* Index into step size table */
};

/* Function prototype for the ADPCM Encoder routine */
char ADPCMEncoder(short, int, struct ADPCMstate *);

/* Function prototype for the ADPCM Decoder routine */
int ADPCMDecoder(char, int, struct ADPCMstate *);


