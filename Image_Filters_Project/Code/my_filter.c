#include "my_filter.h"

int read_ppm_color_bitmap(const char *filename, PPM_Image_Buffer *buf){

  FILE *f = fopen(filename, "r");
  if (f == NULL)
  {
    perror("Could not open file\n");
    return -1;
  }

  char check_format[6];
  fscanf( f,"%s",check_format);
  fseek(f, 8, SEEK_CUR);
  fscanf( f,"%s",&check_format[2]);
  if (strcmp(check_format,"P3255") != 0)
  {
    perror("read_ppm_color_bitmap(): Wrong file format\n");
    return -1;
  }
  //--------------------
  rewind(f);
  fseek(f, 2, SEEK_CUR);

  fscanf(f, "\n%d %d\n", &buf->rown, &buf->coln);

  fseek(f, 3, SEEK_CUR);

  int size = buf->rown * buf->coln;
  buf->data = (Pixel_Data *)malloc(size * sizeof(Pixel_Data));
  if (!buf->data)
  {
    perror("read_ppm_color_bitmap(): Malloc failed\n");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < size; i++)
  {
    fscanf(f, "%u %u %u ", (unsigned *)&buf->data[i].red, (unsigned *)&buf->data[i].green, (unsigned *)&buf->data[i].blue);
  }
  fclose(f);
  return 0;
}

int write_ppm_color_bitmap(const char *filename, PPM_Image_Buffer *buf){
  FILE *f = fopen(filename, "w+");
  if (f == NULL)
  {
    fprintf(stderr,"Could not open file\n");
    return -1;
  }
  //--------------------

  fprintf(f, "P3\n%d %d\n255", buf->rown, buf->coln);
  fprintf(f, "\n");

  for (int i = 0; i < buf->rown * buf->coln; i++)
  {
    fprintf(f, "%u %u %u\n", buf->data[i].red, buf->data[i].green, buf->data[i].blue);
  }
  fclose(f);
  return 0;
}

void filter_color_component(PPM_Image_Buffer *buf, unsigned int rgb_mask){
  int pixels = buf->rown * buf->coln;
  for (int i = 0; i < pixels; i++) // change between pixels
  {
    for (int bit = 0; bit < 3; bit++) // change between the 3 bits
    {
      if (!!(rgb_mask & (1 << bit)) == 0) // check if current bit is 0
      {
        // check the position of the 0 and remove the value for the colour
        if (bit == 0)
          buf->data[i].red = 0;
        else if (bit == 1)
          buf->data[i].green = 0;
        else
          buf->data[i].blue = 0;
      }
    }
  }
}

void convert_to_grayscale(PPM_Image_Buffer *buf){
  unsigned grey = 0;
  int size = buf->rown * buf->coln;
  for (int i = 0; i < size; i++) // change between pixels
  {
    // change the RGB values to grey
    grey = buf->data[i].red * 0.3 + buf->data[i].green * 0.59 + buf->data[i].blue * 0.11;
    buf->data[i].red = grey;
    buf->data[i].green = grey;
    buf->data[i].blue = grey;
  }
}

int P3_to_P6(const char *filename, PPM_Image_Buffer *buf){
  // write in binary file
  FILE *binary_write = fopen(filename, "wb+");

  if (!binary_write)
  {
    perror("Could not open file\n");
    return EXIT_FAILURE;
  }
  //--------------------
  fprintf(binary_write, "P6\n%d %d\n255", buf->rown, buf->coln);
  fprintf(binary_write, "\n");

  for (int i = 0; i < buf->rown * buf->coln; i++)
  {
    fwrite(&buf->data[i], sizeof(buf->data[i]), 1, binary_write); 
  }
  fclose(binary_write);
  return EXIT_SUCCESS;
}

int P6_to_P3(const char *filename, PPM_Image_Buffer *buf){
  // read from binary file
  FILE *binary_read = fopen(filename, "rb");
  if (!binary_read)
  {
    perror("Could not open file\n");
    return EXIT_FAILURE;
  }
  char check_format[6];
  fscanf( binary_read,"%s",check_format);
  fseek(binary_read, 8, SEEK_CUR);
  fscanf( binary_read,"%s",&check_format[2]);
  if (strcmp(check_format,"P6255") != 0)
  {
    perror("read_ppm_color_bitmap(): Wrong file format\n");
    return -1;
  }
  //--------------------
  rewind(binary_read);
  fseek(binary_read, 2, SEEK_CUR);

  fscanf(binary_read, "\n%d %d\n", &buf->rown, &buf->coln);

  fseek(binary_read, 4, SEEK_CUR);

  int size = buf->rown * buf->coln;
  buf->data = (Pixel_Data *)malloc(size * sizeof(Pixel_Data));
  if (!buf->data)
  {
    fprintf(stderr,"P6_to_P3(): Malloc failed\n");
    return EXIT_FAILURE;
  }
  
  fread(buf->data, sizeof(*buf->data), size, binary_read);
 
  fclose(binary_read);
  return EXIT_SUCCESS;
}
