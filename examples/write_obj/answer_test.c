/* This program is used for testing the output of the "write_obj" example
 * It is used when linking "answer.exe", and calls the object produced by the "write_obj" example
 */

#include <stdio.h>

extern int answer();

int main()
{
    int a = answer();
    printf("The answer is %i", a);
}
