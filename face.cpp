////////////////////////////////In The Name of GOD///////////////////////////
// a simple " face detection " project by  "91105903"  &&  "91105466"  ////////////////

#include <stdio.h>
#include <conio.h>
#include <math.h>

FILE *bmpfile;

char filename[50];
int height, width; 
int padding;
struct pixel
{
       unsigned char R, G, B; 
       unsigned char Ra, Ga, Ba;
       float R1, G1, B1;
       float r, g;
       float H, S, L;      
} picture[1000][1000];
int checklabel[1000][1000] = {0};
int REchecklabel[1000][1000] = {1};
int color; // It's last value , shows the number of objects
int omit_obj[20] = {0}; // includes the number of objects which ommited after optional part !

struct objects
{
       int pi; // mokhtasate "x" e yek sare ghotre bounding box ra midahad.
       int pj; // mokhtasate "y" e yek sare ghotre bounding box ra midahad.
       int qi; // mokhtasate "x" e sare digare bounding box ra midahad.
       int qj; // mokhtasate "y" e sare digare bounding box ra midahad.
       int area_box; // the area of bounding boxes
       int area_skin; // the area of objects in the boxes.
       float percentage; 
       float ratio;
       int area_obj_in_obj; // we use it in the optional port
}object[1000] = {0};

int getFileCheckBMP();
void initWidth_Height();
void readPixels();
void copyRGB();
void RGB2HSL();
void normRGB();
void skinPixelClassification();
float findMax(int, int);
float findMin(int, int);
bool checkSkin();
float F1(float);
float F2(float);
float W(float, float);
void makeWhiteBlackPic();
// labelling functions:
void initCheckLabel();
bool findFirstNeg1(int);
int broad(int);
void label();
// End of labelling functions:
void findMaxHeight();
void findMinHeight();
void findMaxWidth();
void findMinWidth();
void setPosition();
void initArea();
void change255_to_0();
void percentage();
void goldenRatio();
void checkArea();
void correct();
void initRECheckLabel();
// optional funtions:
void startOptional();
void findAreaHole();
// END of optional part
void drawBox(); // It makes the final file then draws BOXes on the faces!!!!
int main()
{
    if ( getFileCheckBMP() == 0 )
       return 0;
    initWidth_Height();
    readPixels();
    copyRGB();
    skinPixelClassification();
    label();
    setPosition();
    initArea();
    percentage();
    goldenRatio();
    checkArea();
    change255_to_0();
    correct();
    initRECheckLabel();
    startOptional();
    findAreaHole();
    change255_to_0();
    makeWhiteBlackPic();
    drawBox();
    printf("\nThis Picture contains %d faces.", color);
    printf("\n\nTask Completed.\n");
    getch();
    return 0;
}

int getFileCheckBMP()  //gets file's name and checks whether it's a BMP or not!
{
     printf("Enter your filename or for exit press 0: \n" );
	 char signature[2];
	 gets(filename);
	 if (filename[0] == '0' && filename[1] == '\0')
           return 0;
     bmpfile = fopen(filename, "rb");
	 if(!bmpfile) 
     {
        fclose(bmpfile);
        printf("This file doesn't exist.\n");
        getFileCheckBMP();
     }
     if (filename[0] == '0' && filename[1] == '\0')
           return 0;
     rewind(bmpfile);
	 fread(&signature, sizeof(char), 2, bmpfile);
	 if( signature[0] != 'B' || signature[1] != 'M' )
     {
	     printf("Your file format is not BMP\n" );
	     getFileCheckBMP();
	 }
}
void initWidth_Height()    //initializes width and height of BMP file.
{
     rewind(bmpfile);
     fseek(bmpfile, 18, SEEK_SET);
     fread(&width, sizeof(char), 4, bmpfile);
     fread(&height, sizeof(char), 4, bmpfile);
     
}
void readPixels()           // reads the pixels of bmp file
{     
     padding = 4 - ( (3 * width) % 4 );
     if (padding == 4) padding = 0;
     fseek(bmpfile, 54, SEEK_SET);
     int i, j;
     for (i = 0; i < height; i++)
     {
         for (j = 0; j < width; j++)
         {
             fread(&(picture[i][j].B), sizeof(char), 1, bmpfile);
             fread(&(picture[i][j].G), sizeof(char), 1, bmpfile);
             fread(&(picture[i][j].R), sizeof(char), 1, bmpfile);
         }
         fseek(bmpfile, padding, SEEK_CUR);
     }
}
void copyRGB()  // storing RGB for next use:
{
     int i, j;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)
             {
                  picture[i][j].Ra = picture[i][j].R;
                  picture[i][j].Ga = picture[i][j].G;
                  picture[i][j].Ba = picture[i][j].B;
             }
}
//////////////////////////////////////////////
void RGB2HSL()          // changes RGB to HSL!
{
     int i, j;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)
         {
             picture[i][j].R1 = picture[i][j].R / 255.0;
             picture[i][j].G1 = picture[i][j].G / 255.0;             
             picture[i][j].B1 = picture[i][j].B / 255.0;
             float max1 = findMax(i, j);
             float min1 = findMin(i, j);         
             // meghdardehie h, s, l :
             picture[i][j].L = (max1 + min1) / 2.0;
             if (max1 == min1)
             {
                picture[i][j].S = 0;
                picture[i][j].H = 0;
                picture[i][j].L = round( picture[i][j].L * 240 );
             }
             
             else
             {
                {
                   if (picture[i][j].L < 0.5)
                      picture[i][j].S = (max1 - min1) / (max1 + min1);
                   else
                      picture[i][j].S = (max1 - min1) / (2.0 - max1 - min1);
                   if (picture[i][j].R1 == max1)
                      picture[i][j].H = (picture[i][j].G1 - picture[i][j].B1) / (max1 - min1);
                   else if (picture[i][j].G1 == max1)
                      picture[i][j].H = 2 + (  (picture[i][j].B1 - picture[i][j].R1) / (max1 - min1)  );
                   else if (picture[i][j].B == max1)
                      picture[i][j].H = 4 + (  (picture[i][j].R1 - picture[i][j].G1) / (max1 - min1)  );
                }
                if ( picture[i][j].H < 0)
                   picture[i][j].H += 6;
                picture[i][j].H = round (picture[i][j].H * 40);
                picture[i][j].S = round (picture[i][j].S * 240);
                picture[i][j].L = round (picture[i][j].L * 240);
                }
             }
}

float findMax(int i, int j) // finds the MAX of R, G & B !!!
{
      float max;
      max = picture[i][j].R1;
      if (picture[i][j].G1 > max)
         max = picture[i][j].G1;
      if (picture[i][j].B1 > max)
         max = picture[i][j].B1;
      return max;
}

float findMin(int i, int j)    // finds the MIN of R, G & B !!!
{
      float min;
      min = picture[i][j].R1;
      if (picture[i][j].G1 < min)
         min = picture[i][j].G1;
      if (picture[i][j].B1 < min)
         min = picture[i][j].B1;
      return min;
}
//////////////////////////////////////////////
void normRGB() // Normalizes R  & G.
{
     int i, j;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)
         {
             if ( (picture[i][j].R + picture[i][j].G + picture[i][j].B ) == 0)
                {
                   picture[i][j].r = 0;
                   picture[i][j].g = 0;
                }
            else
               {
                  picture[i][j].r = picture[i][j].R * 1.0 / (picture[i][j].R + picture[i][j].G + picture[i][j].B);
                  picture[i][j].g = picture[i][j].G * 1.0 / (picture[i][j].R + picture[i][j].G + picture[i][j].B);
               }
         }
}
bool checkSkin() // checks the conditions which include HSL and rg 
{
     int i, j;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)
         {
             float F1_r =  F1(picture[i][j].r);
             float F2_r =  F2(picture[i][j].r);
             float W_r_g = W(picture[i][j].r, picture[i][j].g);
             if (picture[i][j].g < F1_r && picture[i][j].g > F2_r && W_r_g > 0.001 && ( picture[i][j].H < 20 || picture[i][j].H >= 239) )
                {
                   if ( picture[i][j].S > 40 && picture[i][j].S < 180 && picture[i][j].L > 40)
                      {
                           picture[i][j].R = 255; 
                           picture[i][j].G = 255;
                           picture[i][j].B = 255;
                      }
                   else 
                   { 
                     picture[i][j].R = 0; 
                     picture[i][j].G = 0;
                     picture[i][j].B = 0;
                   }
                }
                else 
                   { 
                     picture[i][j].R = 0; 
                     picture[i][j].G = 0;
                     picture[i][j].B = 0;
                   }
         }
}
////////////////////////////// rabetehaye marboot be checkSkin :
float F1(float x)
{
      return ( (-1.376 * x * x) + (1.0743 * x) + (0.2) );
}
float F2(float x)
{
      return ( (-0.776 * x * x) + (0.5601 * x) + (0.18) );
}
float W(float x, float y)
{
      return (  (x - 0.33) * (x - 0.33) + (y - 0.33) * (y - 0.33) );
}
///////////////////////////// End of ...
void skinPixelClassification()   // Detects the skin pixels and pixels which look like skin.
{
     RGB2HSL();
     normRGB();
     checkSkin();
}

////////////////////////////////////// Labelling functions :
void initCheckLabel()  // araye ye checklabel ra meghdar dehi mikonad:
{
     int i, j;
     for ( i = 0; i < height; i++)
         for (j = 0; j < width; j++)
             {
                  if (picture[i][j].R == 255)
                     checklabel[i][j] = -1;
             }
}
bool findFirstNeg1(int color) // finds the first -1 in the array:
{
     int i, j;
     for ( i = 0; i < height; i++)
         for (j = 0; j < width; j++)
             {
                  if (checklabel[i][j] == -1)
                  {
                     checklabel[i][j] = color;
                     return true;
                  }
             }
     return false;
}
int broad(int color) // shomareye root ra be hamsayehayash nesbat midahad:
{
    int ch = 0;
    int i, j;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            {
                 int m, n;
                 if (checklabel[i][j] == -1)
                 {
                    for (m = -1; m < 2; m++)
                        for (n = -1; n < 2; n++)
                            if (checklabel[i + m][j + n] == color)
                            {
                               checklabel[i][j] = color;
                               ch++;
                               break;
                            }
                 }
            }
    return ch;
}
void label() // ??
{
     initCheckLabel();
     for (color = 1; true; color++)
         {
                if (!findFirstNeg1(color))
                   return;
                while (broad(color));
         }
}
///////////////////////////////// labelling completed//////////////////////////////////////////////////////////////

void findMaxHeight() // mokhtasseye maximume heighte object: qj
{
     int i, j;
     int obj_num = 1;
     for (i = height - 1; i >= 0; i--)
         for (j = 0; j < width; j++)    
             if (checklabel[i][j] == obj_num)
                {
                                  object[obj_num].qj = i;
                                  obj_num++;
                                  i = height - 1;
                                  break;
                }
}
void findMinHeight() //mokhtasseye minimume heighte object: pj
{
     int i, j;
     int obj_num = 1;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)    
             if (checklabel[i][j] == obj_num)
                {
                                  object[obj_num].pj = i;
                                  obj_num++;
                                  i = 0;
                                  break;
                }
}
void findMinWidth() //mokhtasseye minimume widthe object: pi
{    
     int i, j;
     int obj_num = 1;
     for (i = 0; i < width; i++)
         for (j = 0; j < height; j++)    
             if (checklabel[j][i] == obj_num)
                {
                                  object[obj_num].pi = i;
                                  obj_num++;
                                  i = 0;
                                  break;
                }
}
void findMaxWidth() //mokhtasseye maximume widthe object: qi
{
     int i, j;
     int obj_num = 1;
     for (i = width - 1; i >= 0; i--)
         for (j = 0; j < height; j++)    
             if (checklabel[j][i] == obj_num)
                {
                                  object[obj_num].qi = i;
                                  obj_num++;
                                  i = width - 1;
                                  break;
                }
}
void setPosition()
{
     findMaxHeight();
     findMinHeight();
     findMaxWidth();
     findMinWidth();
}
/////////////////////////////////////////////// payane mokhtassat dehie BOXes.

/////////////////////////////////////////////// initializing the area, percentage & the ratio
void initArea()
{
     int m, i, j;
     for (m = 1; m <= color; m++)
     {
         object[m].area_box = (object[m].qi - object[m].pi) * (object[m].qj - object[m].pj); // initializing the area of bounding box
         // initializing the area of skin
         object[m].area_skin = 0;
         for (i = object[m].pj; i <= object[m].qj; i++)
            for (j = object[m].pi; j <= object[m].qi; j++)
                if (checklabel[i][j] == m)
                   object[m].area_skin++;
     }
}

void percentage() // calculates the percentage of skin in the bounding box :
{
     int m;
     for (m = 1; m <= color; m++)
         object[m].percentage = (object[m].area_skin * 1.0) / (object[m].area_box);
     
}
void goldenRatio()
{
     int m;
     for (m = 1; m <= color; m++)
         object[m].ratio = (object[m].qj - object[m].pj) * 1.0 / (object[m].qi - object[m].pi);
}
/////////////////////////////////////// checks the conditions which include the area, percentage and the ratio
void checkArea()
{
     // checklabel according to the size of area
     int i, m, n;
     for (i = 1; i <= color; i++)
         if (object[i].area_skin < 450 || object[i].area_skin > 4500)
            {
               for (m = 0; m < height; m++)
                   for (n = 0; n < width; n++)
                       {
                            if (checklabel[m][n] == i)
                               checklabel[m][n] = 0;
                       }
            }
     // checklabel according to the percentage  of area in the bounding box
     for (i = 1; i <= color; i++)
         if (object[i].percentage < 0.4 || object[i].percentage > 0.9)
            {
               for (m = 0; m < height; m++)
                   for (n = 0; n < width; n++)
                       {
                            if (checklabel[m][n] == i)
                               checklabel[m][n] = 0;
                       }
            }
     // checklabel according to the ratio of height / width
     for (i = 1; i <= color; i++)
         if (fabs (object[i].ratio - 1.618 ) > 0.4)
            {
               for (m = 0; m < height; m++)
                   for (n = 0; n < width; n++)
                       {
                            if (checklabel[m][n] == i)
                               checklabel[m][n] = 0;
                       }
            }
}
void change255_to_0() // changes RGB according to rhe chekcklabel
{
    int i, j;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            {
                 if (checklabel[i][j] == 0)
                    {
                       picture[i][j].R = 0;
                       picture[i][j].G = 0;
                       picture[i][j].B = 0;
                    }
            }
}
void correct()  // relabels remained objects:
{
     label();
     setPosition();
}
void makeWhiteBlackPic() // creates a white-black pic
{
     FILE *whiteblack;
     whiteblack = fopen("whiteblack.bmp" , "wb");
     rewind(whiteblack);
     rewind(bmpfile);
     unsigned char data;
     int i, j;
     for (i = 0; i < 54; i++)
     {
         fread(&data, sizeof(unsigned char), 1, bmpfile);
         fwrite(&data, sizeof(unsigned char), 1, whiteblack);
     }
     fseek(bmpfile, 54, SEEK_SET);
     fseek(whiteblack, 54, SEEK_SET);
     for (i = 0; i < height; i++)
     {  
         for (j = 0; j < width; j++)
         {
             fwrite(&(picture[i][j].B), sizeof(unsigned char), 1, whiteblack);
             fwrite(&(picture[i][j].G), sizeof(unsigned char), 1, whiteblack);
             fwrite(&(picture[i][j].R), sizeof(unsigned char), 1, whiteblack);
         }
         fwrite("0", sizeof(unsigned char), padding, whiteblack);
     }
    fclose(whiteblack);
}
void initRECheckLabel()
{
     int i, j;
     for (i = 0; i < height; i++)
         for (j = 0; j < width; j++)
             if (checklabel[i][j] != 0)
                REchecklabel[i][j] = 0;
             else       
             REchecklabel[i][j] = 1;
}
////////////////////////////// start of optional part ///////////////////////////////////////////////////
void startOptional()
{
     int m, i, j;
     for (m = 1; m <= color; m++)
     {
         for (i = object[m].pi; i <= object[m].qi; i++)
             for (j = object[m].pj; j <= object[m].qj; j++) 
                 if (REchecklabel[j][i] != 0)
                    REchecklabel[j][i] = 0;
                 else break;
     }     
     for (m = 1; m <= color; m++)
     {
         for (i = object[m].pi; i <= object[m].qi; i++)
             for (j = object[m].qj; j >= object[m].pj; j--) 
                 if (REchecklabel[j][i] != 0)
                    REchecklabel[j][i] = 0;
                 else break;
     }
}
void findAreaHole()
{
     // initializng the area of holes in each object :
     int m, i, j;
     for (m = 1; m <= color; m++)
     {
         object[m].area_obj_in_obj = 0;
         for (i = object[m].pj; i <= object[m].qj; i++)
             for (j = object[m].pi; j <= object[m].qi; j++)
                 if (REchecklabel[i][j] == 1)
                    object[m].area_obj_in_obj++;
     }
     // ommitting the objects
     int k = 0;
     for (m = 1; m <= color; m++)
         if (object[m].area_obj_in_obj < 70)
            {
                for (i = object[m].pj; i <= object[m].qj; i++)
                    for (j = object[m].pi; j <= object[m].qi; j++)
                        checklabel[i][j] = 0;
            omit_obj[k] = m;
            k++;
            }
}
///////////////////////////////////////// END of optional part /////////////////////////////////////////

///////////////////////////////////////// Drawing Bounding BOXes!! ////////////////////////////////////
void drawBox()
{
     int m;
     int i, j;
     int r;
     int ch;
     int change = 0;
     FILE *finalfile;
     finalfile = fopen("Final.bmp" , "wb");
     unsigned char data;
     rewind(bmpfile);
     rewind(finalfile);
     for (i = 0; i < 54; i++)
     {
         fread(&data, sizeof(unsigned char), 1, bmpfile);
         fwrite(&data, sizeof(unsigned char), 1, finalfile);
     }
     for (m = 1; m <= color; m++)
     {
         ch = 0;
         for ( r = 0; r < (sizeof(omit_obj) / sizeof(int)); r++)
             if (m == omit_obj[r])
              {
                   ch = 1;
                   change++;
              }
         if (ch == 1)
            continue;
         for ( i = object[m].pi; i <= object[m].qi; i++)
             {
                 picture[object[m].pj][i].Ra = 255;
                 picture[object[m].pj][i].Ga = 0;
                 picture[object[m].pj][i].Ba = 0;
             }
         for ( i = object[m].pi; i <= object[m].qi; i++)
             {
                 picture[object[m].qj][i].Ra = 255;
                 picture[object[m].qj][i].Ga = 0;
                 picture[object[m].qj][i].Ba = 0;
             }
         for ( j = object[m].pj; j <= object[m].qj; j++)
             {
                 picture[j][object[m].pi].Ra = 255;
                 picture[j][object[m].pi].Ga = 0;
                 picture[j][object[m].pi].Ba = 0;
             }
         for ( j = object[m].pj; j <= object[m].qj; j++)
             {
                 picture[j][object[m].qi].Ra = 255;
                 picture[j][object[m].qi].Ga = 0;
                 picture[j][object[m].qi].Ba = 0;
             }
     }
     fseek(finalfile, 54, SEEK_SET);
     for (i = 0; i < height; i++)
     {  
         for (j = 0; j < width; j++)
         {
             fwrite(&(picture[i][j].Ba), sizeof(unsigned char), 1, finalfile);
             fwrite(&(picture[i][j].Ga), sizeof(unsigned char), 1, finalfile);
             fwrite(&(picture[i][j].Ra), sizeof(unsigned char), 1, finalfile);
         }
         fwrite("0", sizeof(unsigned char), padding, finalfile);
     }
     fclose(finalfile);
     color -= change;
}
////////////////////////////////////////// BY: Alireza Amiri && Ata Fatahi/////////////////////////////////////////////////////////////
