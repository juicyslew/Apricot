
#include <iostream>
#include <cmath>
#include <stdint.h>
#include <map>

std::map<char, double> char2freq = {{'z', 261.625565}, {'x', 293.66}, {'c', 329.63}, {'v', 349.23}, {'b', 392.00}};


int main()
{

    const double R=8000; // sample rate (samples per second)
    //const double C=261.625565; // frequency of middle-C (hertz)
    double C = 200;
    const double F=R/256; // bytebeat frequency of 1*t due to 8-bit truncation (hertz)
    const double V=127; // a volume constant

    

    for ( int t=0; ; t++ )
    {
        uint8_t temp = (sin(t*2*M_PI/R*C)+1)*V; // pure middle C sine wave
        //uint8_t temp = t/F*C; // middle C saw wave (bytebeat style)
        //uint8_t temp = (t*5&t>>1)|(t*3&t>>1); // viznut bytebeat composition
        std::cout<<temp;
    }

}