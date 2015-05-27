/*****************************************************************************
*	Filename: pcspeech.c                                                     *
******************************************************************************
*	Author:  Rodger Richey                                                   *
*	Title:  Senior Applications Engineer                                     *
*	Company: Microchip Technology Incorporated                               *
*	Revision: 0                                                              *
*	Date: 1-11-96                                                            *
*	Compiled using Borland C+ Version 3.1                                    *
******************************************************************************
*	Include files:                                                           *
*		 stdio.h - Standard input/output header file                         *
*		 stdlib.h - Standard library header file                             *
*		 string.h - Standard string header file                              *
*		 pcadpcm.h - ADPCM related information header file (Rev0)            *
******************************************************************************
*	Usage:                                                                   *
*		ADPCM Encode - pcspeech e <infile> <outfile>                         *
*					   <infile> is a 16-bit raw speech file                  *
*					   <outfile> contains the ADPCM codes                    *
*		ADPCM Decode - pcspeech d <infile> <outfile>                         *
*		               <infile> contains the ADPCM codes                     *
*		               <outfile> is a 16-bit raw speech file                 *
******************************************************************************
*	This file contains the code to:                                          *
*		- Open the input and output files                                    *
*   	- Read data from the input file                                      *
*		- Call the encode/decode routines                                    *
*		- Write data to the output file                                      *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcadpcm.h"
#include <string.h>


/*****************************************************************************
*	Usage - this routine prints a how to message for the pcspeech prgm       *
******************************************************************************
*	Input variables:                                                         *
*		None                                                                 *
*	Output variables:                                                        *
*		None                                                                 *
*****************************************************************************/

void Usage(void)
{
	printf("ADPCM Encoder/Decoder -- usage:\n");
	printf("\tEncoder = pcspeech e bits infile outfile\n");
	printf("\tDecoder = pcspeech d bits infile outfile\n");
	exit(1);
}

/*****************************************************************************
*	main - controls file I/O and ADPCM calls                                 *
******************************************************************************
*	Input variables:                                                         *
*		int argc - number of arguements in argv                              *
*		char **argv - pointer to an array of strings                         *                                                             *
*	Return variables:                                                        *
*		None                                                                 *
*****************************************************************************/
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void fourBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);
void fourBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);
void threeBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);
void threeBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);

int main(int argc, char **argv)
{
	int which = 0;;
	int bits = 0;
	struct ADPCMstate state;
	FILE *fpin;
	FILE *fpout;


	state.prevsample=0;
	state.previndex=0;

	/* Determine if this is an encode or decode operation */
	if(argc <= 1)
		Usage();
	else if( strcmp(argv[1],"e")==0 || strcmp(argv[1],"E")==0 )
		which = 0;
	else if( strcmp(argv[1],"d")==0 || strcmp(argv[1],"D")==0 )
		which = 1;
		argc--;
		argv++;
	if(strcmp(argv[1],"3")==0)
		bits = 3;
	else if(strcmp(argv[1],"4")==0)
		bits = 4;
		argc--;
		argv++;
	/* Open input file for processing */
	if(argc <= 1)
		Usage();
	else if( (fpin=fopen(argv[1],"rb"))==NULL)
	{
		printf("ADPCM Encoder/Decoder\n");
		printf("ERROR: Could not open %s for input\n",argv[1]);
		exit(1);
	}
	argc--;
	argv++;

	/* Open output file */
	if(argc <= 1)
	{
		fclose(fpin);
		Usage();
	}

	else if( (fpout=fopen(argv[1],"wb"))==NULL)
	{
		fclose(fpin);
		printf("ADPCM Encoder/Decoder\n");
		printf("ERROR: Could not open %s for output\n",argv[1]);
		exit(1);
	}


	// ADPCM Decoder selected
	if(which)
	{
		printf("ADPCM Decoding in progress...\n");
		/* Read and unpack input codes and process them */
		switch(bits) {
			case 3:
				threeBitDecode(fpin, fpout, &state);
				break;
			case 4:
				fourBitDecode(fpin, fpout, &state);
				break;
		}



		//fourBitDecode(fpin, fpout, &state);
		printf("ADPCM Decoding finished\n");
	}

	// ADPCM Encoder selected
	else
	{
		printf("ADPCM Encoding in progress...\n");
		/* Read input file and process */
		switch(bits) {
			case 3:
				threeBitEncode(fpin, fpout, &state);
				break;
			case 4:
				fourBitEncode(fpin, fpout, &state);
				break;
		}

		//fourBitEncode(fpin, fpout, &state);
		printf("ADPCM Encoding finished\n");
	}

	fclose(fpin);
	fclose(fpout);

	return 0;
}

void threeBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {
	char * codes = (char*) malloc(sizeof(char)*3);
	char threebit[8];

	while (fread(codes, sizeof (char), 3, fpin) == 3) {
			threebit[0] = codes[0] >> 5;
			threebit[1] = codes[0] >> 2;
			threebit[2] = ((codes[0] & 3) << 2) | (codes[1] >> 7);
			threebit[3] = codes[1] >> 4;
			threebit[4] = codes[1] >> 1 & 3;
			threebit[5] = (codes[1] & 1) | (codes[2] >> 6);
			threebit[6] = codes[2] >> 3;
			threebit[7] = codes[2] & 7;

			// Write sample for  3-bits code
			for(int i = 0;i < 8; ++i) {
				short sample = ADPCMDecoder(threebit[i], 3, state);
				fwrite(&sample, sizeof(short), 1, fpout);
			}
		}
	free(codes);
}


void threeBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {

	short sample;
	int shift = 0;
	char code = 0;
	int codes = 0;
	int i = 0;

	while (fread(&sample, sizeof(short), 1, fpin) == 1)
	{
		// Encode sample into 3-bit code
		code = ADPCMEncoder(sample, 3, state);
		// Shift ADPCM 3-bit code
		//long l = strtol(code, 0, 2);
		unsigned char b = code & 0xffl;
		fwrite(&b, 1, 1, fpout);
		i++;
		if(i ==3 )
			return;


		/*shift += 3;

		if(shift == 24) {
			//codes >>= 2 ;
			char codest = reverse((char)codes);
			fwrite(&codest, sizeof (char), 1, fpout);
			return;
			shift = 0;
			codes = 0;
		}*/
	}
}


void fourBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {
	unsigned char code;
	short sample;

	while (fread(&sample, sizeof(short), 1, fpin) == 1)
	{
		// Encode sample into lower 4-bits of code
		code = ADPCMEncoder(sample, 4, state);
		// Move ADPCM code to upper 4-bits
		code = (code << 4) & 0xf0;

		// Read new sample from file
		if(fread(&sample,sizeof(short),1,fpin)==0)
		{
			// No more samples, write code to file
			fwrite(&code,sizeof(char),1,fpout);
			break;
		}
		// Encode sample and save in lower 4-bits of code
		code |= ADPCMEncoder(sample, 4, state);
		// Write code to file, code contains 2 ADPCM codes
		fwrite(&code, sizeof (char), 1, fpout);

	}

}


void fourBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {
	unsigned char code;
	short sample;


	while (fread(&code, sizeof (char), 1, fpin) == 1)
	{
		// Send the upper 4-bits of code to decoder
		sample = ADPCMDecoder((code>>4)&0x0f,4, state);
		// Write sample for upper 4-bits of code
		fwrite(&sample, sizeof(short), 1, fpout);

		// Send the lower 4-bits of code to decoder
		sample = ADPCMDecoder(code&0x0f, 4, state);
		// Write sample for lower 4-bits of code
		fwrite(&sample,sizeof(short),1,fpout);
	}
}



