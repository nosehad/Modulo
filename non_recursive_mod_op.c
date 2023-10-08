#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#include "Utils/err.h"
#include "Utils/String.h"
#include "Utils/Vector.h"
#include "Utils/StringIterator.h"

static unsigned int binarysqrt(int x)
{
   int i = 0;
   for (; x > 0; x >>= 1, ++i)
      ;
   return i;
}

/* returns 1 if there was an overflow */
static unsigned int potenciate(int *x, int n)
{
   if (n == 0)
   {
      *x = 1;
      return 0;
   }
   for (int _ = 1, ix = *x; _ < n; ++_)
   {
      if (__builtin_mul_overflow(*x, ix, x))
         return 1;
   }
   return 0;
}

/* convert 2 potency <p> like 128 to beautiful unicode string like 2⁷ */
static void print2potency(int p)
{
   if (p == 1)
   {
      printf("<td>2^0</td>");
      return;
   }

   unsigned int n = 0;
   /* find exponent n */
   for (unsigned int i = 1; i != p; i <<= 1, ++n)
      ;

   printf("<td>2^%u</td>", n);
}

typedef Vector(unsigned int) _vect_uint32;
static _vect_uint32 *partiateAdditive(unsigned int x, unsigned int *max)
{
   /* create vector of splitted powers by 2*/
   _vect_uint32 r = vect_create(unsigned int);
   unsigned int mp = 0x80000000U;
   while (mp > x)
      mp >>= 1; /* shift bit -> loadup next power by 2*/

   int t = 0; /* sum of factorials (of power by 2) -> at end has to be equal to *x*/
   *max = binarysqrt(mp);

   for (; t != x; mp >>= 1)
   {
      /* check if factorial is valid */
      if ((t + mp) > x)
         continue;

      /* submit new factorial */
      vect_pushback(r, mp);
      t += mp;
   }

   _vect_uint32 *res = (_vect_uint32 *)malloc(sizeof(_vect_uint32));
   memcpy(res, &r, sizeof(_vect_uint32));
   return res;
}

static int vect_contains(_vect_uint32 *vect, unsigned int item)
{
   for (int i = 0; i < vect->_size; ++i)
      if (*(vect->_vect + i) == item)
         return 1;
   return 0;
}

void main(int argc, char**argv)
{
   /* data fields */
   if(argc == 1)
      return;

   unsigned int a;
   unsigned int b;
   unsigned int n;

   StringIterator moderator = itr_create(*(argv + 1));
   /* iterate modulo operation */
   if (itr_getint(&moderator, &a) == ITR_NO_MATCH)
   {
      throwerr("Invalid a (a ∈ ℕ)\n");
   }
   if (itr_char(&moderator, '^') == ITR_NO_MATCH)
   {
      throwerr("Invalid modulo operation (it should be: a^b mod n)\n");
   }
   if (itr_getuint(&moderator, &b) == ITR_NO_MATCH)
   {
      throwerr("Invalid b (it should be: b ∈ ℕ)\n");
   }
   if (itr_jtext(&moderator, ITR_NO_MATCH, 1, "mod", ITR_MATCH) == ITR_NO_MATCH)
   {
      throwerr("Invalid modulo operation (it should be: a^b mod n)\n");
   }
   if (itr_getuint(&moderator, &n) == ITR_NO_MATCH)
   {
      throwerr("Invalid n (it should be: n ∈ ℕ)\n");
   }

   printf("<h1>Modulo Operation with a=%u, b=%u, n=%u</h1>", a, b, n);
   int max_exponent;
   _vect_uint32 *partiatedPotency = partiateAdditive(b, &max_exponent);
   printf("<h2>Factorial-Table (<strong>%u^%u mod %u</strong>)</h2><table><table><tr><th>Exponent e</th><th>%u^e mod %u</th></tr>", a, b, n, a, n);

   _vect_uint32 results = vect_create(unsigned int);
   int last_result;

   for (unsigned int i = 0, t = a, p = 1; i < max_exponent; ++i, t = a, p <<= 1)
   {
      int needed = vect_contains(partiatedPotency, p); /* check if potency is in partiated potency */

      /* create table */
      printf("<tr>");
      /* print pontency */
      print2potency(p);
      /* try potenciating a with the potency */
      if (potenciate(&t, p))
      { /* split t further on overflow */
         int previous_result = last_result;
         last_result = (last_result * last_result) % n;
         printf("<td><span style=\"color: red;\">%u</span> ^ 2 mod %u = <td><span style=\"color: %s;\">%u</span></td></td>", previous_result, n, needed ? "green" : "red", last_result );
      }
      else
      {
         last_result = t % n;
         printf(needed ? "<td><span style=\"color: green;\">%u</span></td>" : "<td><span style=\"color: red;\">%u</span></td>", last_result);
      }

      if(needed) /* add last result to results */  {
         vect_pushback(results, last_result);
      }
      printf("</tr>");
   }
   
   printf("</table><h2>Result (<strong>%u^%u mod %u</strong>)</h2> <p>(<span style=\"color: green;\">%u</span></td>%s", a, b, n, *results._vect, results._size > 1 ? " * " : "");
   unsigned int result;
   unsigned long long product = *results._vect;


   for(unsigned int i = 1; i < results._size-1; ++i) {
      result = *(results._vect + i);
      product *= result;
      printf("<span style=\"color: green;\">%u</span> * ", result);
   }

   if(results._size > 1) 
   {
      result = *(results._vect + results._size - 1);
      product *= result;
      printf("<span style=\"color: green;\">%u</span>", result);
   }
   printf(") = <span style=\"color: green;\">%lld</span><br>", product);
   printf("=> <span style=\"color: green;\">%lld</span> mod %u = <strong>%u</strong></p>\n", product, n, product % n);
}