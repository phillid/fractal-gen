#include <iostream>
#include <stdio.h>
#include <CImg.h>

//#define TILE_WIDTH 200
//#define TILE_HEIGHT 200

//#define IMAGE_WIDTH 12200
//#define IMAGE_HEIGHT 12200

using namespace cimg_library;

int main(int argc, char **argv)
{
	unsigned int TILE_SIZE, IMAGE_SIZE, x, y, tile_x, tile_y;
	char out_name[] = "________.png";

	if (argc != 4)
	{
		printf("%s in_file image_square tile_square\n",argv[0]);
		return 1;
	}

	IMAGE_SIZE = atoi(argv[2]);
	TILE_SIZE = atoi(argv[3]);


	unsigned char *buf = (unsigned char*)malloc(TILE_SIZE);

	FILE *fp = fopen(argv[1],"r");
	if (!fp)
	{
		printf("ERROR: Cannot open file\n");
		return 1;
	}

	CImg<unsigned char> buffer(TILE_SIZE, TILE_SIZE,1,3,1);
	for (tile_y = 0; tile_y < IMAGE_SIZE/TILE_SIZE; tile_y++)
	{
		for (tile_x = 0; tile_x < IMAGE_SIZE/TILE_SIZE; tile_x++)
		{
			// Seek to data for start of this tile
			#define DATA_Y (tile_y * (TILE_SIZE * TILE_SIZE) * (IMAGE_SIZE / TILE_SIZE))
			#define DATA_X (tile_x * TILE_SIZE)

			fseek(fp, DATA_Y+DATA_X, SEEK_SET);

			printf("Creating tile (%d,%d)...\n", tile_x, tile_y);

			for (y = 0; y < TILE_SIZE; y++)
			{
				fread(buf, TILE_SIZE, 1, fp);
				for (x = 0; x < TILE_SIZE; x++)
				{
#ifdef BW
					if (buf[x] % 2)
					{
						buffer(x,y,0) = buffer(x,y,1) = buffer(x,y,2) = 255;
					} else {
						buffer(x,y,0) = buffer(x,y,1) = buffer(x,y,2) = 0;
					}
#else
					buffer(x,y,0) = 0;
					buffer(x,y,1) = buf[x];
					buffer(x,y,2) = 0;
#endif
				}
				// Seek and wrap around to next Y pos for tile
				fseek(fp, IMAGE_SIZE-TILE_SIZE, SEEK_CUR);
			}
			sprintf(out_name,"%d,%d.png",tile_x,tile_y);
			buffer.save(out_name);
		}
	}
	fclose(fp);
	free(buf);
}





















//#ifdef BLACK
//					if (buf[x] == 255)
//					{
//						buffer(x,y,0) = 0;
//						buffer(x,y,1) = 0;
//						buffer(x,y,2) = 0;
//					} else {
//#endif

//#ifdef BLACK
//					}
//#endif
//				printf("%d,%d\n",x,y);
