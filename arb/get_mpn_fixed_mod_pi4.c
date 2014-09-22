/*=============================================================================

    This file is part of ARB.

    ARB is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ARB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARB; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2014 Fredrik Johansson

******************************************************************************/

#include "arb.h"

#define TMP_ALLOC_LIMBS(__n) TMP_ALLOC((__n) * sizeof(mp_limb_t))

int _arf_get_integer_mpn(mp_ptr y, mp_srcptr x, mp_size_t xn, long exp);

int _arf_set_mpn_fixed(arf_t z, mp_srcptr xp, mp_size_t xn, mp_size_t fixn, int negative, long prec);

int
_arb_get_mpn_fixed_mod_pi4(mp_ptr w, fmpz_t q, int * octant,
    mp_limb_t * error, const arf_t x, mp_size_t wn)
{
    mp_srcptr xp;
    mp_size_t xn;
    long exp;

    ARF_GET_MPN_READONLY(xp, xn, x);
    exp = ARF_EXP(x);

    if (exp <= -1)
    {
        flint_mpn_zero(w, wn);
        *error = _arf_get_integer_mpn(w, xp, xn, exp + wn * FLINT_BITS);
        *octant = 0;
        if (q != NULL)
            fmpz_zero(q);
        return 1;
    }
    else
    {
        mp_ptr qp, rp, np;
        mp_srcptr dp;
        mp_size_t qn, rn, nn, dn, tn, alloc;
        TMP_INIT;

        tn = ((exp + 2) + FLINT_BITS - 1) / FLINT_BITS;

        dn = wn + tn;           /* denominator */
        nn = wn + 2 * tn;       /* numerator */
        qn = nn - dn + 1;       /* quotient */
        rn = dn;                /* remainder */

        if (dn > ARB_PI4_TAB_LIMBS)
            return 0;

        TMP_START;

        alloc = qn + rn + nn;
        qp = TMP_ALLOC_LIMBS(alloc);
        rp = qp + qn;
        np = rp + rn;

        dp = arb_pi4_tab + ARB_PI4_TAB_LIMBS - dn;

        flint_mpn_zero(np, nn);
        _arf_get_integer_mpn(np, xp, xn, exp + dn * FLINT_BITS);

        mpn_tdiv_qr(qp, rp, 0, np, nn, dp, dn);

        *octant = qp[0] % 8;

        if (*octant % 2 == 0)
        {
            flint_mpn_copyi(w, rp + tn, wn);
            *error = 2;
        }
        else
        {
            mpn_sub_n(w, dp + tn, rp + tn, wn);
            *error = 3;
        }

        if (q != NULL)
        {
            /* read the exponent */
            while (qn > 1 && qp[qn-1] == 0)
                qn--;

            if (qn == 1)
                fmpz_set_ui(q, qp[0]);
            else
                fmpz_set_mpn_large(q, qp, qn, 0);
        }

        TMP_END;
        return 1;
    }
}

