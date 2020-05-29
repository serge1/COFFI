const char *info = 
    "Test for the Texas Instruments C2000 processors                                               "
    "This source code is compiled qwith the Texas Instruments Code Composer Studio compiler        "
    "For ti_c2000_1.obj:                                                                           "
    "    cl2000 ti_c2000_1.c                                                                       "
    "For ti_c2000_1.out:                                                                           "
    "    cl2000 -z ti_c2000_1.obj -o ti_c2000_1.out                                                "
    "For the dumps:                                                                                "
    "    ofd2000 ti_c2000_1.obj > ti_c2000_1.obj.dump                                              "
    "    ofd2000 ti_c2000_1.out > ti_c2000_1.out.dump                                              "
    ;                                                                                             

const char      c_i01 = -42;
const short     c_i02 = -42;
const int       c_i03 = -42;
const long      c_i04 = -42;
const char      c_i05[17] = {-0, -1, -1, -2, -3, -5, -8, -13, -21, -34, -55, -89, -144, -233, -377, -610, -987};
const short     c_i06[17] = {-0, -1, -1, -2, -3, -5, -8, -13, -21, -34, -55, -89, -144, -233, -377, -610, -987};
const int       c_i07[17] = {-0, -1, -1, -2, -3, -5, -8, -13, -21, -34, -55, -89, -144, -233, -377, -610, -987};
const long      c_i08[17] = {-0, -1, -1, -2, -3, -5, -8, -13, -21, -34, -55, -89, -144, -233, -377, -610, -987};
const long long c_i09[17] = {-0, -1, -1, -2, -3, -5, -8, -13, -21, -34, -55, -89, -144, -233, -377, -610, -987};

const unsigned char      c_u01 = 42;
const unsigned short     c_u02 = 42;
const unsigned int       c_u03 = 42;
const unsigned long      c_u04 = 42;
const unsigned char      c_u05[17] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987};
const unsigned short     c_u06[17] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987};
const unsigned int       c_u07[17] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987};
const unsigned long      c_u08[17] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987};
const unsigned long long c_u09[17] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987};

char      i01;
short     i02;
int       i03;
long      i04;
char      i05[17];
short     i06[17];
int       i07[17];
long      i08[17];
long long i09[17];

unsigned char      u01;
unsigned short     u02;
unsigned int       u03;
unsigned long      u04;
unsigned char      u05[17];
unsigned short     u06[17];
unsigned int       u07[17];
unsigned long      u08[17];
unsigned long long u09[17];

const float       g01 = 42.42;
const double      g02 = 42.42;
const long double g03 = 42.42;

float       h01;
double      h02;
long double h03;

signed char      j01;
signed short     j02;
signed int       j03;
signed long      j04;
signed long long j09;

const char *str = "azerty";

struct S01 {
	char  S01_f01;
	short S01_f02;
	long  S01_f03;
} s01;

typedef struct {
	char  S02_f01;
	union {
		char  S02_f02_f01;
		short S02_f02_f02;
		long  S02_f02_f03;
		struct {
			char  S02_f02_f04_f01;
			short S02_f02_f04_f02;
			long  S02_f02_f04_f03;
		} f04;
	} S02_f02;
} S02;

S02 s02;
const S02 c_s02 = {1, {2}};

enum E01 {
	E01v01,
	E01v02
} e01;

typedef enum {
	E02v01,
	E02v02
} E02;

E02 e02;

main()
{
	volatile int x;
	
	x = c_i05[0];
	x = c_i06[0];
	x = c_i07[0];
	x = c_i08[0];
	x = c_i09[0];
	
	x = c_u05[0];
	x = c_u06[0];
	x = c_u07[0];
	x = c_u08[0];
	x = c_u09[0];
	
	x = c_s02.S02_f01;
}
