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
void fiveBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);
void fiveBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state);
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
	else if(strcmp(argv[1], "5")==0)
		bits = 5;
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
			case 5:
				fiveBitDecode(fpin, fpout, &state);
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
			case 5:
				fiveBitEncode(fpin, fpout, &state);
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


			threebit[0] = (codes[0] >> 5) & 0x7;
			threebit[1] = (codes[0] >> 2) & 0x7;
			threebit[2] = (((codes[0] & 3) << 1) | ((codes[1] >> 7) & 0x1));
			threebit[3] = (codes[1] >> 4) & 0x7;
			threebit[4] = (codes[1] >> 1) & 0x7;
			threebit[5] = (((codes[1] & 1) << 2) | ((codes[2] >> 6) & 0x3)) & 0x7;
			threebit[6] = (codes[2] >> 3) & 0x7;
			threebit[7] = codes[2] & 0x7;

			// Write sample for  3-bits code
			for(int i = 0;i < 8; ++i) {
				short sample = ADPCMDecoder(threebit[i], 3, state);
				fwrite(&sample, sizeof(short), 1, fpout);
			}
		}
	free(codes);
}


void threeBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {

	signed short sample;
	char code = 0;
	int i = 0;
	char buffer[3] = {0};

	char hold = 0;
	while (fread(&sample, sizeof(short), 1, fpin) == 1)
	{

		code = ADPCMEncoder(sample, 3, state);

		switch(i) {

		case 0:
			buffer[0] = code << 5;
			break;
		case 1:
			buffer[0] |= code << 2;
			break;
		case 2:
			hold = code;
			buffer[0] |= (code >> 1);
			break;
		case 3:
			buffer[1] = hold << 7;
			buffer[1] |= code << 4;
			break;
		case 4:
			buffer[1] |= code << 1;
			break;
		case 5:
			hold = code;
			buffer[1] |= (code >> 2);
			break;
		case 6:
			buffer[2] = hold << 6;
			buffer[2] |= code << 3;
			break;
		case 7:
			buffer[2] |= code;
		}

		i++;
		if(i == 8) {
			fwrite(&buffer, sizeof (char) * 3, 1, fpout);
			i = 0;
		}
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

void fiveBitEncode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {
	unsigned char code;
	short sample;

	while (fread(&sample, sizeof(short), 1, fpin) == 1)
	{
		// Encode sample into lower 4-bits of code
		code = ADPCMEncoder(sample, 5, state);
		// Move ADPCM code to upper 4-bits
		/*code = (code << 4) & 0xf0;

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
		fwrite(&code, sizeof (char), 1, fpout);*/

	}

}


void fiveBitDecode(FILE  *fpin, FILE  *fpout, struct ADPCMstate *state) {
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

