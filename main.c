#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define ROWS		(int)480
#define COLUMNS		(int)640
#define PI          3.14159265358979323846
void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );
int getCoordinate(int i);
int findMax(float *[ROWS]);
int getDeDx(unsigned char image[ROWS][COLUMNS], unsigned char ximage[ROWS][COLUMNS]);
int getDeDy(unsigned char image[ROWS][COLUMNS], unsigned char yimage[ROWS][COLUMNS]);
int getSGM(unsigned char ximage[ROWS][COLUMNS], unsigned char yimage[ROWS][COLUMNS], unsigned char sgm[ROWS][COLUMNS]);
void getBinary(unsigned char sgm[ROWS][COLUMNS],unsigned char biImage[ROWS][COLUMNS],int threshold);
int main( int argc, char **argv )
{
	int				i,k, threshold, max[3],p,j;
	float           buffer[500];
	FILE			*fp;
	unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], SGM[ROWS][COLUMNS], biIMAGE[ROWS][COLUMNS], head[32];
	char			filename[50], ifilename[50], ch;
    strcpy ( filename, "image.raw" );
	//memset ( voting, 0, sizeof(int) * 180 * 400 );
	header ( ROWS, COLUMNS, head );

    clear( ximage );
    clear( yimage );
    clear( biIMAGE);
		/* Read in the image */
	if (!( fp = fopen( filename, "rb" ) ))
	{
		fprintf( stderr, "error: couldn't open %s\n", argv[1] );
		exit(1);
	}

	for ( i = 0 ; i < ROWS ; i++ )
		if (!( COLUMNS == fread( image[i], sizeof(char), COLUMNS, fp ) ))
		{
			fprintf( stderr, "error: couldn't read %s\n", argv[1] );
			exit(1);
		}
	fclose(fp);
    printf("This assignment is too ");
    max[0] = 0; //maximum of Gx
    max[1] = 0; //maximum of Gy
    max[2] = 0; //maximum of SGM

    /* Compute Gx, Gy, SGM, find out the maximum and normalize*/

    /*****************************************
    ************** CALCULATE Gx **************
    ******************************************/
    max[0]=getDeDx(image, ximage);
    /* Write Gx to a new image*/
    strcpy( ifilename, filename );
    if (!( fp = fopen( strcat( ifilename, "-x.ras" ), "wb" ) ))
    {
      fprintf( stderr, "error: could not open %s\n", ifilename );
      exit( 1 );
    }
    fwrite( head, 4, 8, fp );

    for ( i = 0 ; i < ROWS ; i++ ) fwrite( ximage[i], 1, COLUMNS, fp );
    fclose( fp );

    /*****************************************
    ************** CALCULATE Gy **************
    ******************************************/
    max[1]=getDeDy(image,yimage);
            /* Write Gy to a new image */
    strcpy( ifilename, filename );
    if (!( fp = fopen( strcat( ifilename, "-y.ras" ), "wb" ) ))
    {
      fprintf( stderr, "error: could not open %s\n", ifilename );
      exit( 1 );
    }
    fwrite( head, 4, 8, fp );

    for ( i = 0 ; i < ROWS ; i++ ) fwrite( yimage[i], 1, COLUMNS, fp );
    fclose( fp );

    /*****************************************
    ************** CALCULATE SGM **************
    ******************************************/
    max[2]=getSGM(ximage,yimage,SGM);
    /* Write SGM to a new image */
    strcpy( ifilename, filename );
    if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
    {
      fprintf( stderr, "error: could not open %s\n", ifilename );
      exit( 1 );
    }
    fwrite( head, 4, 8, fp );


    for ( i = 0 ; i < ROWS ; i++ ) fwrite( SGM[i], 1, COLUMNS, fp );
    fclose( fp );

    /*****************************************
    ********* CALCULATE BINARY IMAGE *********
    ******************************************/
    threshold=44;
    getBinary(SGM,biIMAGE,threshold);
    printf("Dung is crazy.");
    /* Write the binary image to a new image */
    strcpy( ifilename, filename );
    if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
    {
      fprintf( stderr, "error: could not open %s\n", ifilename );
      exit( 1 );
    }
    fwrite( head, 4, 8, fp );

    for ( i = 0 ; i < ROWS ; i++ ) fwrite( biIMAGE[i], 1, COLUMNS, fp );
    fclose( fp );
    /*****************************************
    ********* Hough transform ****************
    ******************************************/
    for(j=0;j<=180;j++)
    {
        for(i=0;i<ROWS;i++)
        {
            for(k=0;k<COLUMNS;k++)
            {
                if(biIMAGE[i][k]==255)
                {
                    buffer[i]= k*cos(j/180*PI)-i*sin(j/180*PI);
                    printf("angle= %d buffer[%d]= %f \n",j,i,buffer[i]);
                }


            }
        }
    }

	return 0;
}
int getSGM(unsigned char ximage[ROWS][COLUMNS], unsigned char yimage[ROWS][COLUMNS], unsigned char sgm[ROWS][COLUMNS])
{
    int max=0;
    int i,j;
    float *buffer[ROWS];
    for(i=0;i<ROWS;i++)
    {
        buffer[i]=(float*) malloc(COLUMNS*sizeof(float));
    }
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
            buffer[i][j]=ximage[i][j]*ximage[i][j]+yimage[i][j]*yimage[i][j];
        }
    }
    max=findMax(buffer);

    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
                sgm[i][j]=(int)((buffer[i][j]/max)*255);
        }
    }
    for(i=0;i<ROWS;i++)
    {
        free(buffer[i]);
    }
    return max;
}

void getBinary(unsigned char sgm[ROWS][COLUMNS],unsigned char biImage[ROWS][COLUMNS],int threshold)
{
    int i,j;
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
            if(sgm[i][j]>threshold)
                biImage[i][j]=255;
            else
                biImage[i][j]=0;
        }
    }
}

int getDeDx(unsigned char image[ROWS][COLUMNS],unsigned char ximage[ROWS][COLUMNS])
{
    int i,j,max=0;
    float *buffer[ROWS];
    for(i=0;i<ROWS;i++)
    {
        buffer[i]=(float*) malloc(COLUMNS*sizeof(float));
    }
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {

            if((i==0) || (i== ROWS-1) || (j==0) || (j==COLUMNS-1))
            {
                continue;
            }
            else
            {
                    buffer[i][j] =abs(  image[i-1][j-1]*-1+
                                        image[i][j-1]*-2+
                                        image[i+1][j-1]*-1+
                                        image[i-1][j]*0+
                                        image[i][j]*0+
                                        image[i+1][j]*0+
                                        image[i-1][j+1]*1+
                                        image[i][j+1]*2+
                                        image[i+1][j+1]*1   );
            }
        }
    }
    max=findMax(buffer);
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
            ximage[i][j]=(int)((buffer[i][j]/max)*255);
        }
    }
    for(i=0;i<ROWS;i++)
    {
        free(buffer[i]);
    }
    return max;
}

int getDeDy(unsigned char image[ROWS][COLUMNS],unsigned char yimage[ROWS][COLUMNS])
{
    int i,j,max=0;
    float *buffer[ROWS];
    for(i=0;i<ROWS;i++)
    {
        buffer[i]=(float*) malloc(COLUMNS*sizeof(float));
    }
    for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                if((i==0) || (i== ROWS-1) || (j==0) || (j==COLUMNS-1))
                {
                    continue;
                }
                else
                {
                    buffer[i][j]=abs(image[i-1][j-1]*-1+
						image[i][j-1]*0+
						image[i+1][j-1]*1+
						image[i-1][j]*-2+
						image[i][j]*0+
						image[i+1][j]*2+
						image[i-1][j+1]*-1+
						image[i][j+1]*0+
						image[i+1][j+1]*1);
                }

            }
        }

        max=findMax(buffer);

        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                    yimage[i][j]=(int)(((float)buffer[i][j]/(float)max)*255);
            }
        }
    return max;
}

int getCoordinate(int i)
{
    if(i==0)
        return -1;
    else if(i==1)
        return 0;
    else if(i==2)
        return 1;
    else
        return 10000;
}
int findMax(float *image[ROWS])
{
    int max=0;
    int i,j;
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
            if(max<(int)image[i][j])
                max=(int)image[i][j];
        }
    }
    return max;
}
void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */

	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++;
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++;
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;

	ch = (char*)&num;
	head[19] = *ch;
	ch ++;
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;


	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}
