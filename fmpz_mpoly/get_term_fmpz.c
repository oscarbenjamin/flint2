/*
    Copyright (C) 2017 William Hart
    Copyright (C) 2017 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <gmp.h>
#include <stdlib.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_mpoly.h"

void fmpz_mpoly_get_term_fmpz(fmpz_t c, const fmpz_mpoly_t poly,
                                 ulong const * exp, const fmpz_mpoly_ctx_t ctx)
{
   slong N, index, exp_bits;
   ulong maskhi, masklo;
   ulong * packed_exp;
   int exists, deg, rev;

   TMP_INIT;

   degrev_from_ord(deg, rev, ctx->ord);

   /* compute how many bits are required to represent exp */
   exp_bits = mpoly_exp_bits(exp, ctx->n, deg);
   if (exp_bits > FLINT_BITS)
       flint_throw(FLINT_EXPOF, "Exponent overflow in fmpz_mpoly_get_term_fmpz");

   if (exp_bits > poly->bits) /* exponent too large to be poly exponent */
   {
      fmpz_zero(c);
      return;
   }

   TMP_START;
   
   masks_from_bits_ord(maskhi, masklo, poly->bits, ctx->ord);
   N = mpoly_words_per_exp(poly->bits, ctx->minfo);

   packed_exp = (ulong *) TMP_ALLOC(N*sizeof(ulong));

   /* pack exponent vector */
   mpoly_set_monomial_ui(packed_exp, exp, poly->bits, ctx->minfo);

   /* work out at what index term should be placed */
   exists = mpoly_monomial_exists(&index, poly->exps,
                                  packed_exp, poly->length, N, maskhi, masklo);

   if (!exists) /* term with that exponent doesn't exist */
      fmpz_zero(c);
   else  /* term with that monomial exists */
      fmpz_mpoly_get_coeff_fmpz(c, poly, index, ctx);

   TMP_END; 
}
