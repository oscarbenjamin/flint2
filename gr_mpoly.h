/*
    Copyright (C) 2016-2017 William Hart
    Copyright (C) 2017-2020 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#ifndef GR_MPOLY_H
#define GR_MPOLY_H

#ifdef GR_MPOLY_INLINES_C
#define GR_MPOLY_INLINE FLINT_DLL
#else
#define GR_MPOLY_INLINE static __inline__
#endif

#include "flint/fmpz_mpoly.h"
#include "gr.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    gr_ptr coeffs;
    ulong * exps;
    slong length;
    flint_bitcnt_t bits;    /* number of bits per exponent */
    slong coeffs_alloc;     /* abs size in mp_limb_t units */
    slong exps_alloc;       /* abs size in ulong units */
}
gr_mpoly_struct;

typedef gr_mpoly_struct gr_mpoly_t[1];


/* Memory management */

GR_MPOLY_INLINE
void gr_mpoly_init(gr_mpoly_t A, const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    A->coeffs = NULL;
    A->exps = NULL;
    A->length = 0;
    A->bits = MPOLY_MIN_BITS;
    A->coeffs_alloc = 0;
    A->exps_alloc = 0;
}

void gr_mpoly_init3(gr_mpoly_t A, slong alloc, flint_bitcnt_t bits,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

void gr_mpoly_init2( gr_mpoly_t A, slong alloc,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

GR_MPOLY_INLINE
void gr_mpoly_clear(gr_mpoly_t A, const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    _gr_vec_clear(A->coeffs, A->coeffs_alloc, cctx);

    if (A->coeffs_alloc > 0)
        flint_free(A->coeffs);

    if (A->exps_alloc > 0)
        flint_free(A->exps);
}

void _gr_mpoly_fit_length(
    gr_ptr * coeffs,
    slong * coeffs_alloc,
    ulong ** exps,
    slong * exps_alloc,
    slong N,
    slong length,
    gr_ctx_t cctx);

void gr_mpoly_fit_length(gr_mpoly_t A, slong len, const mpoly_ctx_t mctx, gr_ctx_t cctx);

void gr_mpoly_fit_length_fit_bits(
    gr_mpoly_t A,
    slong len,
    flint_bitcnt_t bits,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

void gr_mpoly_fit_length_reset_bits(
    gr_mpoly_t A,
    slong len,
    flint_bitcnt_t bits,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

/* todo: when to zero out coefficients? */
GR_MPOLY_INLINE
void _gr_mpoly_set_length(gr_mpoly_t A, slong newlen, 
                                               const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    FLINT_ASSERT(newlen <= A->coeffs_alloc);
    FLINT_ASSERT(mpoly_words_per_exp(A->bits, mctx)*newlen <= A->exps_alloc);

    A->length = newlen;
}

/* Basic manipulation */

GR_MPOLY_INLINE
void gr_mpoly_swap(gr_mpoly_t A, gr_mpoly_t B, const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    gr_mpoly_struct t = *A;
    *A = *B;
    *B = t;
}

int gr_mpoly_set(gr_mpoly_t A, const gr_mpoly_t B, const mpoly_ctx_t mctx, gr_ctx_t cctx);

GR_MPOLY_INLINE
int gr_mpoly_zero(gr_mpoly_t A, const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    _gr_mpoly_set_length(A, 0, mctx, cctx);
    return GR_SUCCESS;
}

GR_MPOLY_INLINE
truth_t gr_mpoly_is_zero(const gr_mpoly_t A, const mpoly_ctx_t mctx, gr_ctx_t cctx)
{
    if (A->length == 0)
        return T_TRUE;

    /* todo: skip when we have canonical representation */
    return _gr_vec_is_zero(A->coeffs, A->length, cctx);
}

truth_t gr_mpoly_equal(const gr_mpoly_t A, const gr_mpoly_t B, const mpoly_ctx_t mctx, gr_ctx_t cctx);

/* Container operations */

void _gr_mpoly_push_exp_ui(
    gr_mpoly_t A,
    const ulong * exp,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

int gr_mpoly_push_term_scalar_ui(gr_mpoly_t A,
    gr_srcptr c,
    const ulong * exp,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

void _gr_mpoly_push_exp_fmpz(
    gr_mpoly_t A,
    const fmpz * exp,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

int gr_mpoly_push_term_scalar_fmpz(
    gr_mpoly_t A,
    gr_srcptr c,
    const fmpz * exp,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

void gr_mpoly_sort_terms(gr_mpoly_t A, const mpoly_ctx_t mctx, gr_ctx_t cctx);

int gr_mpoly_combine_like_terms(
    gr_mpoly_t A,
    const mpoly_ctx_t mctx, gr_ctx_t cctx);

/* Random generation */

int gr_mpoly_randtest_bits(gr_mpoly_t A, flint_rand_t state, slong length, flint_bitcnt_t exp_bits, const mpoly_ctx_t mctx, gr_ctx_t cctx);

/* Input and output */

int gr_mpoly_write_pretty(gr_stream_t out, const gr_mpoly_t A,
                             const char ** x_in, const mpoly_ctx_t mctx, gr_ctx_t cctx);

int gr_mpoly_print_pretty(const gr_mpoly_t A,
                             const char ** x_in, const mpoly_ctx_t mctx, gr_ctx_t cctx);

/* Arithmetic */

int gr_mpoly_neg(gr_mpoly_t A, const gr_mpoly_t B, const mpoly_ctx_t mctx, gr_ctx_t cctx);
int gr_mpoly_add(gr_mpoly_t A, const gr_mpoly_t B, const gr_mpoly_t C, const mpoly_ctx_t mctx, gr_ctx_t cctx);
int gr_mpoly_sub(gr_mpoly_t A, const gr_mpoly_t B, const gr_mpoly_t C, const mpoly_ctx_t mctx, gr_ctx_t cctx);

int gr_mpoly_mul(gr_mpoly_t poly1, const gr_mpoly_t poly2, const gr_mpoly_t poly3, const mpoly_ctx_t mctx, gr_ctx_t cctx);
int gr_mpoly_mul_johnson(gr_mpoly_t poly1, const gr_mpoly_t poly2, const gr_mpoly_t poly3, const mpoly_ctx_t mctx, gr_ctx_t cctx);

#ifdef __cplusplus
}
#endif

#endif
