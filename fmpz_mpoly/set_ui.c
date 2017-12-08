/*
    Copyright (C) 2016 William Hart
    Copyright (C) 2017 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpz_mpoly.h"

void fmpz_mpoly_set_ui(fmpz_mpoly_t poly, ulong c, const fmpz_mpoly_ctx_t ctx)
{
    slong N = mpoly_words_per_exp(poly->bits, ctx->minfo);

    if (c == 0)
    {
        _fmpz_mpoly_set_length(poly, 0, ctx);
        return;
    }

    N = mpoly_words_per_exp(poly->bits, ctx->minfo);

    fmpz_mpoly_fit_length(poly, 1, ctx);
    fmpz_set_ui(poly->coeffs + 0, c);
    mpoly_monomial_zero(poly->exps + N*0, N);
    _fmpz_mpoly_set_length(poly, 1, ctx);
}
