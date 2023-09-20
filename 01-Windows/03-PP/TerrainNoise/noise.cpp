#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define MAXB 0x100
#define N 0x1000

#define s_curve(t) ( t * t * (3.0 - 2.0 * t))
#define lerp(t, a, b) ( a + t * (b - a))	//(1 - t) * a + t * b

#define setup(i, b0, b1, r0, r1) \
			 t = vec[i] + N; \
			 b0 = ((int)t) & BM; \
			 b1 = (b0 + 1) & BM; \
			 r0 = t - (int)t; \
			 r1 = r0 - 1.0;

#define at2( rx, ry) ( rx * q[0] + ry * q[1])
#define at3( rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2])

static int p[MAXB + MAXB + 2];
static double g3[MAXB + MAXB + 2][3];
static double g2[MAXB + MAXB + 2][2];
static double g1[MAXB + MAXB + 2];

static int start;
static int B;
static int BM;


//
//normalize2()
//
void normalize2(double v[2])
{
	//variable  declaration
	double s;

	//code
	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

//
//normalize3()
//
void normalize3(double v[3])
{
	//variable  declaration
	double s;

	//code
	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

//
//initNoise()
//
void initNoise(void)
{
	//variable declaration
	int i, j, k;

	//code
	srand(30757);

	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (double)((rand() % (B + B)) - B) / B;

		for( j = 0; j < 2; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for( j = 0; j < 3; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];

		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}


//
//SetNoiseFrequency()
//
void SetNoiseFrequency(int frequency)
{
	//code
	start = 1;
	B = frequency;
	BM = B - 1;
}

//
//noise3()
//
double noise3(double vec[3])
{
	//variable declaration
	int bx0, bx1,
		by0, by1,
		bz0, bz1;
	int b00, b10, b01, b11;

	double rx0, rx1,
		   ry0, ry1,
		   rz0, rz1;
	double *q, sy, sz, a, b, c, d, t, u, v;

	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);


	q = g3[b00 + bz0];
	u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0];
	v = at3(rx1, ry0, rz0);
	a = lerp(t, u, v);

	q = g3[b01 + bz0];
	u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0];
	v = at3(rx1, ry1, rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);


	q = g3[b00 + bz1];
	u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1];
	v = at3(rx1, ry0, rz1);
	a = lerp(t, u, v);

	q = g3[b01 + bz1];
	u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1];
	v = at3(rx1, ry1, rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return(lerp(sz, c, d));
}

//
//make3DNoise()
//
void make3DNoise(int noiseTexSize, unsigned char **noisePtr)	//noisePtr must be null
{
	//variable declaration
	int f, i, j, k, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	int frequency = startFrequency;

	double ni[3];
	double inci, incj, inck;
	double amplitude = 0.5;

	unsigned char *ptr;

	//code
	*noisePtr = (unsigned char *)malloc(noiseTexSize * noiseTexSize * noiseTexSize * 4);

	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amplitude *= 0.5)
	{
		SetNoiseFrequency(frequency);
		ptr = *noisePtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (noiseTexSize / frequency);
		for (i = 0; i < noiseTexSize; ++i, ni[0] += inci)
		{
			incj = 1.0 / (noiseTexSize / frequency);
			for (j = 0; j < noiseTexSize; ++j, ni[1] += incj)
			{
				inck = 1.0 / (noiseTexSize / frequency);
				for (k = 0; k < noiseTexSize; ++k, ni[2] += inck, ptr += 4)
				{
					*(ptr + inc) = (unsigned char)(((noise3(ni) + 1.0) * amplitude) * 255.0);
				}
			}
		}
	}
}

