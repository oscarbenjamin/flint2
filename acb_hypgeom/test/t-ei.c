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

    Copyright (C) 2015 Fredrik Johansson

******************************************************************************/

#include "acb_hypgeom.h"

void
acb_hypgeom_ei_fallback(acb_t res, const acb_t z, slong prec)
{
    acb_t t, u;

    acb_init(t);
    acb_init(u);

    acb_zero(t);
    acb_neg(u, z);
    acb_hypgeom_gamma_upper(u, t, u, 0, prec);
    acb_neg(u, u);

    acb_log(t, z, prec);
    acb_mul_2exp_si(t, t, -1);
    acb_add(u, u, t, prec);

    acb_inv(t, z, prec);
    acb_log(t, t, prec);
    acb_mul_2exp_si(t, t, -1);
    acb_sub(u, u, t, prec);

    acb_neg(t, z);
    acb_log(t, t, prec);
    acb_sub(u, u, t, prec);

    acb_swap(res, u);

    acb_clear(t);
    acb_clear(u);
}

int main()
{
    slong iter;
    flint_rand_t state;

    printf("ei....");
    fflush(stdout);

    flint_randinit(state);

    for (iter = 0; iter < 3000; iter++)
    {
        acb_t z0, z1, w0, w1;
        slong prec0, prec1;

        acb_init(z0);
        acb_init(z1);
        acb_init(w0);
        acb_init(w1);

        prec0 = 2 + n_randint(state, 1000);
        prec1 = 2 + n_randint(state, 1000);

        acb_randtest(z0, state, 1 + n_randint(state, 1000), 1 + n_randint(state, 100));
        acb_randtest(w0, state, 1 + n_randint(state, 1000), 1 + n_randint(state, 100));
        acb_randtest(w1, state, 1 + n_randint(state, 1000), 1 + n_randint(state, 100));

        acb_set(z1, z0);
        if (n_randint(state, 2))
        {
            acb_add(z1, z1, w0, prec0);
            acb_sub(z1, z1, w0, prec0);
        }

        switch (n_randint(state, 4))
        {
            case 0:
                acb_hypgeom_ei_2f2(w0, z0, prec0);
                break;
            case 1:
                acb_hypgeom_ei_asymp(w0, z0, prec0);
                break;
            case 2:
                acb_hypgeom_ei(w0, z0, prec0);
                break;
            default:
                acb_hypgeom_ei_fallback(w0, z0, prec0);
        }

        switch (n_randint(state, 4))
        {
            case 0:
                acb_hypgeom_ei_2f2(w1, z1, prec1);
                break;
            case 1:
                acb_hypgeom_ei_asymp(w1, z1, prec1);
                break;
            case 2:
                acb_hypgeom_ei(w1, z1, prec1);
                break;
            default:
                acb_hypgeom_ei_fallback(w1, z1, prec1);
        }

        if (!acb_overlaps(w0, w1))
        {
            printf("FAIL: consistency\n\n");
            printf("z0 = "); acb_printd(z0, 30); printf("\n\n");
            printf("z1 = "); acb_printd(z1, 30); printf("\n\n");
            printf("w0 = "); acb_printd(w0, 30); printf("\n\n");
            printf("w1 = "); acb_printd(w1, 30); printf("\n\n");
            abort();
        }

        acb_clear(z0);
        acb_clear(z1);
        acb_clear(w0);
        acb_clear(w1);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return EXIT_SUCCESS;
}

