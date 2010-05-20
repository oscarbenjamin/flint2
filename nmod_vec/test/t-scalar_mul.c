/*============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

===============================================================================*/
/****************************************************************************

   Copyright (C) 2009 William Hart

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "nmod_vec.h"
#include "ulong_extras.h"

int main(void)
{
   int result;
   printf("scalar_mul....");
   fflush(stdout);
   
   // check (a + b)*c == a*c + b*c
   for (ulong i = 0; i < 10000UL; i++) 
   {
      ulong j;
	  
	  ulong length = n_randint(100) + 1;
	  mp_limb_t * vec = nmod_vec_init(length);
	  mp_limb_t * vec2 = nmod_vec_init(length);
	  mp_limb_t * vec3 = nmod_vec_init(length);

	  mp_limb_t n = n_randtest_not_zero();
	  mp_limb_t c = n_randint(n);
	  nmod_t mod;
	  nmod_init(&mod, n);

      _nmod_vec_randtest(vec, length, mod);
      _nmod_vec_randtest(vec2, length, mod);

	  _nmod_vec_add(vec3, vec, vec2, length, mod);
	  _nmod_vec_scalar_mul(vec3, vec3, length, mod, c);

	  _nmod_vec_scalar_mul(vec, vec, length, mod, c);
	  _nmod_vec_scalar_mul(vec2, vec2, length, mod, c);
	  _nmod_vec_add(vec, vec, vec2, length, mod);
	  
	  if (!_nmod_vec_equal(vec, vec3, length))
	  {
	     printf("FAIL\n");
		 printf("length = %ld, n = %ld\n", length, n);
		 abort();
	  }

	  nmod_vec_free(vec);
	  nmod_vec_free(vec2);
	  nmod_vec_free(vec3);
   }

   printf("PASS\n");
   return 0;
}
