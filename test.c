#include "gr.h"

typedef int ((*gr_test_function)(gr_ctx_t, flint_rand_t, int));

int
gr_test_binary_op_aliasing(gr_ctx_t R, int (*gr_op)(gr_ptr, gr_srcptr, gr_srcptr, gr_ctx_t), flint_rand_t state, int verbose)
{
    int status, equal, alias;
    GR_TMP_START;
    gr_ptr x, y, xy1, xy2;

    GR_TMP_INIT4(x, y, xy1, xy2, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_op(xy1, x, y, R);

    alias = n_randint(state, 4);
    switch (alias)
    {
        case 0:
            status |= gr_op(xy1, x, y, R);
            gr_set(xy2, x, R);
            status |= gr_op(xy2, xy2, y, R);
            break;
        case 1:
            status |= gr_op(xy1, x, y, R);
            gr_set(xy2, y, R);
            status |= gr_op(xy2, x, y, R);
            break;
        case 2:
            gr_set(y, x, R);
            status |= gr_op(xy1, x, y, R);
            status |= gr_op(xy2, x, x, R);
            break;
        default:
            gr_set(y, x, R);
            gr_set(xy2, x, R);
            status |= gr_op(xy1, x, y, R);
            status |= gr_op(xy2, xy2, xy2, R);
    }

    status |= gr_equal(&equal, xy1, xy2, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("alias: %d\n", alias);
        printf("x = "); gr_println(x, R);
        printf("y = "); gr_println(y, R);
        printf("y (op) y (1) = "); gr_println(xy1, R);
        printf("x (op) y (2) = "); gr_println(xy2, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, y, xy1, xy2, R);
    GR_TMP_END;

    return status;
}

int
gr_test_binary_op_type_variants(gr_ctx_t R,
    int (*gr_op)(gr_ptr, gr_srcptr, gr_srcptr, gr_ctx_t),
    int (*gr_op_ui)(gr_ptr, gr_srcptr, ulong, gr_ctx_t),
    int (*gr_op_si)(gr_ptr, gr_srcptr, slong, gr_ctx_t),
    int (*gr_op_fmpz)(gr_ptr, gr_srcptr, const fmpz_t, gr_ctx_t),
    int (*gr_op_fmpq)(gr_ptr, gr_srcptr, const fmpq_t, gr_ctx_t),
    flint_rand_t state, int verbose)
{
    int status, equal, alias, which;
    GR_TMP_START;
    gr_ptr x, y, xy1, xy2;
    ulong uy;
    slong sy;
    fmpz_t zy;
    fmpq_t qy;

    GR_TMP_INIT4(x, y, xy1, xy2, R);
    fmpz_init(zy);
    fmpq_init(qy);

    which = equal = 0;

    uy = n_randtest(state);
    sy = (slong) n_randtest(state);
    fmpz_randtest(zy, state, 100);
    fmpq_randtest(qy, state, 100);

    for (which = 0; which < 4; which++)
    {
        gr_randtest(x, state, NULL, R);
        gr_randtest(y, state, NULL, R);
        gr_randtest(xy1, state, NULL, R);
        gr_randtest(xy2, state, NULL, R);

        status = GR_SUCCESS;
        alias = n_randint(state, 2);

        if (which == 0)
        {
            if (alias)
            {
                status |= gr_set(xy1, x, R);
                status |= gr_op_ui(xy1, xy1, uy, R);
            }
            else
            {
                status |= gr_op_ui(xy1, x, uy, R);
            }
            status |= gr_set_ui(y, uy, R);
            status |= gr_op(xy2, x, y, R);
            status |= gr_equal(&equal, xy1, xy2, R);
        }
        else if (which == 1)
        {
            if (alias)
            {
                status |= gr_set(xy1, x, R);
                status |= gr_op_si(xy1, xy1, sy, R);
            }
            else
            {
                status |= gr_op_si(xy1, x, sy, R);
            }
            status |= gr_set_si(y, sy, R);
            status |= gr_op(xy2, x, y, R);
            status |= gr_equal(&equal, xy1, xy2, R);
        }
        else if (which == 2)
        {
            if (alias)
            {
                status |= gr_set(xy1, x, R);
                status |= gr_op_fmpz(xy1, xy1, zy, R);
            }
            else
            {
                status |= gr_op_fmpz(xy1, x, zy, R);
            }
            status |= gr_set_fmpz(y, zy, R);
            status |= gr_op(xy2, x, y, R);
            status |= gr_equal(&equal, xy1, xy2, R);
        }
        else
        {
            if (alias)
            {
                status |= gr_set(xy1, x, R);
                status |= gr_op_fmpq(xy1, xy1, qy, R);
            }
            else
            {
                status |= gr_op_fmpq(xy1, x, qy, R);
            }
            status |= gr_set_fmpq(y, qy, R);
            status |= gr_op(xy2, x, y, R);
            status |= gr_equal(&equal, xy1, xy2, R);
        }

        if (status == GR_SUCCESS && !equal)
        {
            status = GR_WRONG;
            break;
        }
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("which: %d\n", which);
        printf("alias: %d\n", alias);
        printf("x = "); gr_println(x, R);
        printf("uy = %lu\n", uy);
        printf("y = "); gr_println(y, R);
        printf("y (op) y (1) = "); gr_println(xy1, R);
        printf("x (op) y (2) = "); gr_println(xy2, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, y, xy1, xy2, R);
    GR_TMP_END;

    fmpz_clear(zy);
    fmpq_clear(qy);

    return status;
}

int
gr_test_init_clear(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status;
    GR_TMP_START;
    gr_ptr a, b, c, d, e;

    status = GR_SUCCESS;

    GR_TMP_INIT1(a, R);
    status |= gr_randtest(a, state, NULL, R);
    GR_TMP_CLEAR1(a, R);

    GR_TMP_INIT2(a, b, R);
    status |= gr_randtest(a, state, NULL, R);
    status |= gr_randtest(b, state, NULL, R);
    GR_TMP_CLEAR2(a, b, R);

    GR_TMP_INIT3(a, b, c, R);
    status |= gr_randtest(a, state, NULL, R);
    status |= gr_randtest(b, state, NULL, R);
    status |= gr_randtest(c, state, NULL, R);
    GR_TMP_CLEAR3(a, b, c, R);

    GR_TMP_INIT4(a, b, c, d, R);
    status |= gr_randtest(a, state, NULL, R);
    status |= gr_randtest(b, state, NULL, R);
    status |= gr_randtest(c, state, NULL, R);
    status |= gr_randtest(d, state, NULL, R);
    GR_TMP_CLEAR4(a, b, c, d, R);

    GR_TMP_INIT5(a, b, c, d, e, R);
    status |= gr_randtest(a, state, NULL, R);
    status |= gr_randtest(b, state, NULL, R);
    status |= gr_randtest(c, state, NULL, R);
    status |= gr_randtest(d, state, NULL, R);
    status |= gr_randtest(e, state, NULL, R);
    GR_TMP_CLEAR5(a, b, c, d, e, R);

    GR_TMP_END;

    return status;
}

int
gr_test_swap(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status;
    GR_TMP_START;
    gr_ptr a, b, c, d;
    int equal0, equal1, equal2, equal3, equal4;

    status = GR_SUCCESS;

    GR_TMP_INIT4(a, b, c, d, R);

    status |= gr_randtest(a, state, NULL, R);
    status |= gr_randtest(b, state, NULL, R);
    status |= gr_set(c, a, R);
    status |= gr_set(d, b, R);

    status |= gr_swap(a, a, R);
    status |= gr_equal(&equal0, a, c, R);

    status |= gr_swap(a, b, R);
    status |= gr_equal(&equal1, b, c, R);
    status |= gr_equal(&equal2, a, d, R);

    status |= gr_swap(a, b, R);
    status |= gr_equal(&equal3, a, c, R);
    status |= gr_equal(&equal4, b, d, R);

    if (status == GR_SUCCESS && (!equal0 || !equal1 || !equal2 || !equal3 || !equal4))
    {
        status = GR_WRONG;
    }

    GR_TMP_CLEAR4(a, b, c, d, R);

    GR_TMP_END;

    return status;
}

int
gr_test_zero_one(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status;
    GR_TMP_START;
    gr_ptr a;
    int equal;

    status = GR_SUCCESS;

    GR_TMP_INIT1(a, R);

    status |= gr_randtest(a, state, NULL, R);
    status |= gr_zero(a, R);
    status |= gr_is_zero(&equal, a, R);
    if (status == GR_SUCCESS && !equal)
        status = GR_WRONG;

    status |= gr_randtest(a, state, NULL, R);
    status |= gr_one(a, R);
    status |= gr_is_one(&equal, a, R);
    if (status == GR_SUCCESS && !equal)
        status = GR_WRONG;

    GR_TMP_CLEAR1(a, R);

    GR_TMP_END;

    return status;
}

int
gr_test_add_associativity(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, z;
    gr_ptr xy, yz, xy_z, x_yz;

    GR_TMP_INIT3(x, y, z, R);
    GR_TMP_INIT4(xy, yz, xy_z, x_yz, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);
    gr_randtest(z, state, NULL, R);
    gr_randtest(xy, state, NULL, R);
    gr_randtest(yz, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_add(xy, x, y, R);
    status |= gr_add(yz, y, z, R);
    status |= gr_add(xy_z, xy, z, R);
    status |= gr_add(x_yz, x, yz, R);
    status |= gr_equal(&equal, xy_z, x_yz, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("z = \n"); gr_println(z, R);
        printf("x + y = \n"); gr_println(xy, R);
        printf("y + z = \n"); gr_println(yz, R);
        printf("(x + y) + z = \n"); gr_println(xy_z, R);
        printf("x + (y + z) = \n"); gr_println(x_yz, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, y, z, R);
    GR_TMP_CLEAR4(xy, yz, xy_z, x_yz, R);
    GR_TMP_END;

    return status;
}

int
gr_test_neg(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, xy;

    GR_TMP_INIT3(x, y, xy, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);
    gr_randtest(xy, state, NULL, R);

    status = GR_SUCCESS;

    /* check x + (-x) = 0 */
    status |= gr_neg(y, x, R);
    status |= gr_add(xy, x, y, R);
    status |= gr_is_zero(&equal, xy, R);
    if (status == GR_SUCCESS && !equal)
        status = GR_WRONG;

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("x + y = \n"); gr_println(xy, R);
        printf("\n");
    }

    /* check -(-x) = x, plus aliasing */
    status |= gr_neg(y, y, R);
    status |= gr_equal(&equal, x, y, R);
    if (status == GR_SUCCESS && !equal)
        status = GR_WRONG;

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, y, xy, R);
    GR_TMP_END;

    return status;
}

int
gr_test_add_commutativity(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, xy, yx;

    GR_TMP_INIT4(x, y, xy, yx, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_add(xy, x, y, R);
    status |= gr_add(yx, y, x, R);
    status |= gr_equal(&equal, xy, yx, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("y + y = \n"); gr_println(xy, R);
        printf("y + x = \n"); gr_println(yx, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, y, xy, yx, R);
    GR_TMP_END;

    return status;
}

int
gr_test_add_aliasing(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_aliasing(R, gr_add, state, verbose);
}

int
gr_test_add_type_variants(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_type_variants(R,
        gr_add, gr_add_ui, gr_add_si, gr_add_fmpz, gr_add_fmpq,
            state, verbose);
}

int
gr_test_sub_equal_neg_add(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, neg_y, x_sub_y, x_neg_y;

    GR_TMP_INIT5(x, y, neg_y, x_sub_y, x_neg_y, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);
    gr_randtest(neg_y, state, NULL, R);
    gr_randtest(x_sub_y, state, NULL, R);
    gr_randtest(x_neg_y, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_sub(x_sub_y, x, y, R);
    status |= gr_neg(neg_y, y, R);
    status |= gr_add(x_neg_y, x, neg_y, R);
    status |= gr_equal(&equal, x_sub_y, x_neg_y, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("-y = \n"); gr_println(neg_y, R);
        printf("x - y = \n"); gr_println(x_sub_y, R);
        printf("x + (-y) = \n"); gr_println(x_neg_y, R);
        printf("\n");
    }

    GR_TMP_CLEAR5(x, y, neg_y, x_sub_y, x_neg_y, R);
    GR_TMP_END;

    return status;
}

int
gr_test_sub_aliasing(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_aliasing(R, gr_sub, state, verbose);
}

int
gr_test_sub_type_variants(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_type_variants(R,
        gr_sub, gr_sub_ui, gr_sub_si, gr_sub_fmpz, gr_sub_fmpq,
            state, verbose);
}


int
gr_test_mul_associativity(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, z;
    gr_ptr xy, yz, xy_z, x_yz;

    GR_TMP_INIT3(x, y, z, R);
    GR_TMP_INIT4(xy, yz, xy_z, x_yz, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);
    gr_randtest(z, state, NULL, R);
    gr_randtest(xy, state, NULL, R);
    gr_randtest(yz, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_mul(xy, x, y, R);
    status |= gr_mul(yz, y, z, R);
    status |= gr_mul(xy_z, xy, z, R);
    status |= gr_mul(x_yz, x, yz, R);
    status |= gr_equal(&equal, xy_z, x_yz, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("z = \n"); gr_println(z, R);
        printf("x * y = \n"); gr_println(xy, R);
        printf("y * z = \n"); gr_println(yz, R);
        printf("(x * y) * z = \n"); gr_println(xy_z, R);
        printf("x * (y * z) = \n"); gr_println(x_yz, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, y, z, R);
    GR_TMP_CLEAR4(xy, yz, xy_z, x_yz, R);
    GR_TMP_END;

    return status;
}

int
gr_test_mul_commutativity(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, y, xy, yx;

    GR_TMP_INIT4(x, y, xy, yx, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_mul(xy, x, y, R);
    status |= gr_mul(yx, y, x, R);
    status |= gr_equal(&equal, xy, yx, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        printf("y * y = \n"); gr_println(xy, R);
        printf("y * x = \n"); gr_println(yx, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, y, xy, yx, R);
    GR_TMP_END;

    return status;
}

int
gr_test_mul_aliasing(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_aliasing(R, gr_mul, state, verbose);
}

int
gr_test_mul_type_variants(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_type_variants(R,
        gr_mul, gr_mul_ui, gr_mul_si, gr_mul_fmpz, gr_mul_fmpq,
            state, verbose);
}

int
gr_test_div_type_variants(gr_ctx_t R, flint_rand_t state, int verbose)
{
    return gr_test_binary_op_type_variants(R,
        gr_div, gr_div_ui, gr_div_si, gr_div_fmpz, gr_div_fmpq,
            state, verbose);
}

int
gr_test_inv_involution(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    gr_ptr x, x_inv, x_inv_inv;

    GR_TMP_INIT3(x, x_inv, x_inv_inv, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(x_inv, state, NULL, R);
    gr_randtest(x_inv_inv, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_inv(x_inv, x, R);
    status |= gr_inv(x_inv_inv, x_inv, R);
    status |= gr_equal(&equal, x, x_inv_inv, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("x ^ -1 = \n"); gr_println(x_inv, R);
        printf("(x ^ -1) ^ -1 = \n"); gr_println(x_inv_inv, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, x_inv, x_inv_inv, R);
    GR_TMP_END;

    return status;
}

int
gr_test_inv_multiplication(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal1, equal2;
    GR_TMP_START;
    gr_ptr x, x_inv, x_inv_x, x_x_inv;

    GR_TMP_INIT4(x, x_inv, x_inv_x, x_x_inv, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(x_inv, state, NULL, R);
    gr_randtest(x_inv_x, state, NULL, R);
    gr_randtest(x_x_inv, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_inv(x_inv, x, R);
    status |= gr_mul(x_inv_x, x_inv, x, R);
    status |= gr_mul(x_x_inv, x, x_inv, R);
    status |= gr_is_one(&equal1, x_inv_x, R);
    status |= gr_is_one(&equal2, x_x_inv, R);

    if (status == GR_SUCCESS && (!equal1 || !equal2))
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("x ^ -1 = \n"); gr_println(x_inv, R);
        printf("(x ^ -1) * x = \n"); gr_println(x_inv_x, R);
        printf("x * (x ^ -1) = \n"); gr_println(x_x_inv, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, x_inv, x_inv_x, x_x_inv, R);
    GR_TMP_END;

    return status;
}

int
gr_test_pow_ui_exponent_addition(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    ulong a, b;
    gr_ptr x, xa, xb, xab, xaxb;

    GR_TMP_INIT5(x, xa, xb, xab, xaxb, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(xa, state, NULL, R);
    gr_randtest(xb, state, NULL, R);
    gr_randtest(xab, state, NULL, R);
    gr_randtest(xaxb, state, NULL, R);

    if (R->flags & GR_FINITE_RING)
    {
        do {
            a = n_randtest(state);
            b = n_randtest(state);
        } while (a + b < a);
    }
    else
    {
        a = n_randtest(state) % 256;
        b = n_randtest(state) % 256;
    }

    status = GR_SUCCESS;

    status |= gr_pow_ui(xa, x, a, R);
    status |= gr_pow_ui(xb, x, b, R);
    status |= gr_pow_ui(xab, x, a + b, R);
    status |= gr_mul(xaxb, xa, xb, R);
    status |= gr_equal(&equal, xab, xaxb, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        flint_printf("a = %wu\n", a);
        flint_printf("b = %wu\n", b);
        printf("x ^ a = \n"); gr_println(xa, R);
        printf("x ^ b = \n"); gr_println(xb, R);
        printf("x ^ (a + b) = \n"); gr_println(xab, R);
        printf("(x ^ a) * (x ^ b) = \n"); gr_println(xaxb, R);
        printf("\n");
    }

    GR_TMP_CLEAR5(x, xa, xb, xab, xaxb, R);
    GR_TMP_END;

    return status;
}

int
gr_test_pow_ui_base_scalar_multiplication(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    ulong a;
    slong y;
    gr_ptr x, xa, ya, xya, xaya;

    GR_TMP_INIT3(x, xa, ya, R);
    GR_TMP_INIT2(xya, xaya, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(xa, state, NULL, R);
    gr_randtest(ya, state, NULL, R);

    y = n_randtest(state);

    if (R->flags & GR_FINITE_RING)
        a = n_randtest(state);
    else
        a = n_randtest(state) % 256;

    status = GR_SUCCESS;
    status |= gr_pow_ui(xa, x, a, R);
    status |= gr_set_si(ya, y, R);
    status |= gr_pow_ui(ya, ya, a, R);
    status |= gr_set_si(xya, y, R);
    status |= gr_mul(xya, x, xya, R);   /* todo mul_si */
    status |= gr_pow_ui(xya, xya, a, R);
    status |= gr_mul(xaya, xa, ya, R);

    status |= gr_equal(&equal, xya, xaya, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        flint_printf("y = %wd\n", y);
        flint_printf("a = %wu\n", a);
        printf("x ^ a = \n"); gr_println(xa, R);
        printf("y ^ a = \n"); gr_println(ya, R);
        printf("(x * y) ^ a = \n"); gr_println(xya, R);
        printf("(x ^ a) * (y ^ a) = \n"); gr_println(xaya, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, xa, ya, R);
    GR_TMP_CLEAR2(xya, xaya, R);
    GR_TMP_END;

    return status;
}

int
gr_test_pow_ui_base_multiplication(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    ulong a;
    gr_ptr x, y, xa, ya, xya, xaya;

    GR_TMP_INIT4(x, y, xa, ya, R);
    GR_TMP_INIT2(xya, xaya, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(y, state, NULL, R);
    gr_randtest(xa, state, NULL, R);
    gr_randtest(ya, state, NULL, R);

    if (R->flags & GR_FINITE_RING)
        a = n_randtest(state);
    else
        a = n_randtest(state) % 256;

    status = GR_SUCCESS;
    status |= gr_pow_ui(xa, x, a, R);
    status |= gr_pow_ui(ya, y, a, R);
    status |= gr_mul(xya, x, y, R);
    status |= gr_pow_ui(xya, xya, a, R);
    status |= gr_mul(xaya, xa, ya, R);

    status |= gr_equal(&equal, xya, xaya, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("y = \n"); gr_println(y, R);
        flint_printf("a = %wu\n", a);
        printf("x ^ a = \n"); gr_println(xa, R);
        printf("y ^ a = \n"); gr_println(ya, R);
        printf("(x * y) ^ a = \n"); gr_println(xya, R);
        printf("(x ^ a) * (y ^ a) = \n"); gr_println(xaya, R);
        printf("\n");
    }

    GR_TMP_CLEAR4(x, y, xa, ya, R);
    GR_TMP_CLEAR2(xya, xaya, R);
    GR_TMP_END;

    return status;
}

int
gr_test_pow_ui_aliasing(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    ulong a;
    gr_ptr x, xa1, xa2;

    GR_TMP_INIT3(x, xa1, xa2, R);

    gr_randtest(x, state, NULL, R);
    gr_randtest(xa1, state, NULL, R);

    if (R->flags & GR_FINITE_RING)
        a = n_randtest(state);
    else
        a = n_randtest(state) % 256;

    status = GR_SUCCESS;
    status |= gr_pow_ui(xa1, x, a, R);
    status |= gr_set(xa2, x, R);
    status |= gr_pow_ui(xa2, xa2, a, R);

    status |= gr_equal(&equal, xa1, xa2, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        flint_printf("a = %wu\n", a);
        printf("x ^ a (1) = \n"); gr_println(xa1, R);
        printf("x ^ a (2) = \n"); gr_println(xa2, R);
        printf("\n");
    }

    GR_TMP_CLEAR3(x, xa1, xa2, R);
    GR_TMP_END;

    return status;
}

int
gr_test_pow_fmpz_exponent_addition(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    GR_TMP_START;
    fmpz_t a, b, ab;
    gr_ptr x, xa, xb, xab, xaxb;

    GR_TMP_INIT5(x, xa, xb, xab, xaxb, R);

    fmpz_init(a);
    fmpz_init(b);
    fmpz_init(ab);

    gr_randtest(x, state, NULL, R);
    gr_randtest(xa, state, NULL, R);
    gr_randtest(xb, state, NULL, R);
    gr_randtest(xab, state, NULL, R);
    gr_randtest(xaxb, state, NULL, R);

    if (R->flags & GR_FINITE_RING)
    {
        fmpz_randtest(a, state, 100);
        fmpz_randtest(b, state, 100);
    }
    else
    {
        fmpz_randtest(a, state, 8);
        fmpz_randtest(b, state, 8);
    }

    fmpz_add(ab, a, b);

    status = GR_SUCCESS;

    status |= gr_pow_fmpz(xa, x, a, R);
    status |= gr_pow_fmpz(xb, x, b, R);
    status |= gr_pow_fmpz(xab, x, ab, R);
    status |= gr_mul(xaxb, xa, xb, R);
    status |= gr_equal(&equal, xab, xaxb, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        printf("\n");
        printf("x = \n"); gr_println(x, R);
        printf("a = "); fmpz_print(a); printf("\n");
        printf("b = "); fmpz_print(b); printf("\n");
        printf("x ^ a = \n"); gr_println(xa, R);
        printf("x ^ b = \n"); gr_println(xb, R);
        printf("x ^ (a + b) = \n"); gr_println(xab, R);
        printf("(x ^ a) * (x ^ b) = \n"); gr_println(xaxb, R);
        printf("\n");
    }

    fmpz_clear(a);
    fmpz_clear(b);
    fmpz_clear(ab);

    GR_TMP_CLEAR5(x, xa, xb, xab, xaxb, R);
    GR_TMP_END;

    return status;
}

int
gr_test_vec_add(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, aliasing, equal;
    slong i, len;
    GR_TMP_START;
    gr_ptr x, y, xy1, xy2;

    len = n_randint(state, 10);

    GR_TMP_INIT_VEC(x, len, R);
    GR_TMP_INIT_VEC(y, len, R);
    GR_TMP_INIT_VEC(xy1, len, R);
    GR_TMP_INIT_VEC(xy2, len, R);

    _gr_vec_randtest(x, state, len, NULL, R);

    _gr_vec_randtest(y, state, len, NULL, R);
    _gr_vec_randtest(xy1, state, len, NULL, R);
    _gr_vec_randtest(xy2, state, len, NULL, R);

    status = GR_SUCCESS;

    aliasing = n_randint(state, 4);
    switch (aliasing)
    {
        case 0:
            status |= _gr_vec_set(xy1, x, len, R);
            status |= _gr_vec_add(xy1, xy1, y, len, R);
            break;
        case 1:
            status |= _gr_vec_set(xy1, y, len, R);
            status |= _gr_vec_add(xy1, x, xy1, len, R);
            break;
        case 2:
            status |= _gr_vec_set(y, x, len, R);
            status |= _gr_vec_set(xy1, x, len, R);
            status |= _gr_vec_add(xy1, xy1, xy1, len, R);
            break;
        default:
            status |= _gr_vec_add(xy1, x, y, len, R);
    }

    for (i = 0; i < len; i++)
        status |= gr_add(GR_ENTRY(xy2, i, R->sizeof_elem),
                         GR_ENTRY(x, i, R->sizeof_elem),
                         GR_ENTRY(y, i, R->sizeof_elem), R);

    status |= _gr_vec_equal(&equal, xy1, xy2, len, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        /* todo: vec print */
        printf("\n");
        printf("aliasing: %d\n", aliasing);
        printf("\n");
    }

    GR_TMP_CLEAR_VEC(x, len, R);
    GR_TMP_CLEAR_VEC(y, len, R);
    GR_TMP_CLEAR_VEC(xy1, len, R);
    GR_TMP_CLEAR_VEC(xy2, len, R);
    GR_TMP_END;

    return status;
}

/* (AB)C = A(BC) */
int
gr_test_mat_mul_classical_associativity(gr_ctx_t R, flint_rand_t state, int verbose)
{
    int status, equal;
    gr_mat_t A, B, C, AB, BC, AB_C, A_BC;
    slong m, n, p, q;

    if (R->flags & GR_FINITE_RING)
    {
        m = n_randint(state, 5);
        n = n_randint(state, 5);
        p = n_randint(state, 5);
        q = n_randint(state, 5);
    }
    else
    {
        m = n_randint(state, 3);
        n = n_randint(state, 3);
        p = n_randint(state, 3);
        q = n_randint(state, 3);
    }

    gr_mat_init(A, m, n, R);
    gr_mat_init(B, n, p, R);
    gr_mat_init(C, p, q, R);
    gr_mat_init(AB, m, p, R);
    gr_mat_init(BC, n, q, R);
    gr_mat_init(AB_C, m, q, R);
    gr_mat_init(A_BC, m, q, R);

    gr_mat_randtest(A, state, NULL, R);
    gr_mat_randtest(B, state, NULL, R);
    gr_mat_randtest(C, state, NULL, R);
    gr_mat_randtest(AB, state, NULL, R);
    gr_mat_randtest(BC, state, NULL, R);
    gr_mat_randtest(AB_C, state, NULL, R);
    gr_mat_randtest(A_BC, state, NULL, R);

    status = GR_SUCCESS;
    status |= gr_mat_mul_classical(AB, A, B, R);
    status |= gr_mat_mul_classical(BC, B, C, R);
    status |= gr_mat_mul_classical(AB_C, AB, C, R);
    status |= gr_mat_mul_classical(A_BC, A, BC, R);
    status |= gr_mat_equal(&equal, AB_C, A_BC, R);

    if (status == GR_SUCCESS && !equal)
    {
        status = GR_WRONG;
    }

    if (verbose || status == GR_WRONG)
    {
        /* todo: vec print */
        printf("\n");
        printf("A = \n"); gr_mat_print(A, R); printf("\n");
        printf("B = \n"); gr_mat_print(B, R); printf("\n");
        printf("C = \n"); gr_mat_print(C, R); printf("\n");
        printf("AB = \n"); gr_mat_print(AB, R); printf("\n");
        printf("BC = \n"); gr_mat_print(BC, R); printf("\n");
        printf("AB * C = \n"); gr_mat_print(AB_C, R); printf("\n");
        printf("A * BC = \n"); gr_mat_print(A_BC, R); printf("\n");
        printf("\n");
    }

    gr_mat_clear(A, R);
    gr_mat_clear(B, R);
    gr_mat_clear(C, R);
    gr_mat_clear(AB, R);
    gr_mat_clear(BC, R);
    gr_mat_clear(A_BC, R);
    gr_mat_clear(AB_C, R);

    return status;
}

void
gr_test_iter(gr_ctx_t R, flint_rand_t state, const char * descr, gr_test_function func, slong iters)
{
    slong iter, count_success, count_unable, count_domain;
    int status;
    timeit_t timer;

    count_success = 0;
    count_unable = 0;
    count_domain = 0;

    printf("%s ... ", descr);
    fflush(stdout);

    timeit_start(timer);

    for (iter = 0; iter < iters; iter++)
    {
        /* flint_printf("iter %ld\n", iter); */
        status = func(R, state, 0);

        if (status == GR_SUCCESS)
            count_success++;

        if (status & GR_UNABLE)
            count_unable++;

        if (status & GR_DOMAIN)
            count_domain++;

        if (status & GR_WRONG)
        {
            flint_printf("\nFAIL\n");
            abort();
        }
    }

    timeit_stop(timer);

    flint_printf("PASS   (%wd successful, %wd domain, %wd unable, 0 wrong, %.3g cpu, %.3g wall)\n",
        count_success, count_domain, count_unable, timer->cpu*0.001, timer->wall*0.001);
}

void
gr_test_ring(gr_ctx_t R, slong iters)
{
    timeit_t timer;
    flint_rand_t state;

    timeit_start(timer);

    flint_randinit(state);

    flint_printf("===============================================================================\n");
    flint_printf("Testing "); gr_ctx_println(R);
    flint_printf("-------------------------------------------------------------------------------\n");

    gr_test_iter(R, state, "init/clear", gr_test_init_clear, iters);
    gr_test_iter(R, state, "swap", gr_test_swap, iters);
    gr_test_iter(R, state, "zero_one", gr_test_zero_one, iters);
    gr_test_iter(R, state, "neg", gr_test_neg, iters);

    gr_test_iter(R, state, "add: associativity", gr_test_add_associativity, iters);
    gr_test_iter(R, state, "add: commutativity", gr_test_add_commutativity, iters);
    gr_test_iter(R, state, "add: aliasing", gr_test_add_aliasing, iters);
    gr_test_iter(R, state, "sub: equal neg add", gr_test_sub_equal_neg_add, iters);
    gr_test_iter(R, state, "sub: aliasing", gr_test_sub_aliasing, iters);

    gr_test_iter(R, state, "add: ui/si/fmpz/fmpq", gr_test_add_type_variants, iters);
    gr_test_iter(R, state, "sub: ui/si/fmpz/fmpq", gr_test_sub_type_variants, iters);
    gr_test_iter(R, state, "mul: ui/si/fmpz/fmpq", gr_test_mul_type_variants, iters);
    gr_test_iter(R, state, "div: ui/si/fmpz/fmpq", gr_test_div_type_variants, iters);

    gr_test_iter(R, state, "mul: associativity", gr_test_mul_associativity, iters);
    if ((R-> flags & GR_COMMUTATIVE_RING) == GR_COMMUTATIVE_RING)
        gr_test_iter(R, state, "mul: commutativity", gr_test_mul_commutativity, iters);
    gr_test_iter(R, state, "mul: aliasing", gr_test_mul_aliasing, iters);

    gr_test_iter(R, state, "inv: multiplication", gr_test_inv_multiplication, iters);
    gr_test_iter(R, state, "inv: involution", gr_test_inv_involution, iters);

    gr_test_iter(R, state, "pow_ui: exponent addition", gr_test_pow_ui_exponent_addition, iters);
    gr_test_iter(R, state, "pow_ui: base scalar multiplication", gr_test_pow_ui_base_scalar_multiplication, iters);

    if ((R-> flags & GR_COMMUTATIVE_RING) == GR_COMMUTATIVE_RING)
        gr_test_iter(R, state, "pow_ui: base multiplication", gr_test_pow_ui_base_multiplication, iters);

    gr_test_iter(R, state, "pow_ui: aliasing", gr_test_pow_ui_exponent_addition, iters);
    gr_test_iter(R, state, "pow_fmpz: exponent addition", gr_test_pow_fmpz_exponent_addition, iters);

    gr_test_iter(R, state, "vec_add", gr_test_vec_add, iters);

    gr_test_iter(R, state, "mat_mul_classical: associativity", gr_test_mat_mul_classical_associativity, iters);

    flint_randclear(state);

    timeit_stop(timer);

    flint_printf("-------------------------------------------------------------------------------\n");
    flint_printf("Tests finished in %.3g cpu, %.3g wall\n", timer->cpu*0.001, timer->wall*0.001);
    flint_printf("===============================================================================\n\n");
}

int main()
{
    gr_ctx_t QQbar, QQbar_real;
    gr_ctx_t QQ;
    gr_ctx_t MQQ;
    gr_ctx_t Zn;
    gr_ctx_t MZn;

    gr_ctx_init_fmpq(QQ);
    QQ->size_limit = 1000;
    gr_test_ring(QQ, 10000);
    gr_ctx_init_matrix(MQQ, QQ, 4);
    MQQ->size_limit = 10;
    gr_test_ring(MQQ, 10);

    gr_ctx_clear(QQ);
    gr_ctx_clear(MQQ);

    gr_ctx_init_nmod8(Zn, 107);
    gr_test_ring(Zn, 1000);

    gr_ctx_init_matrix(MZn, Zn, 10);
    gr_test_ring(MZn, 10);
    gr_ctx_clear(MZn);

    gr_ctx_init_matrix(MZn, Zn, 4);
    gr_test_ring(MZn, 1000);
    gr_ctx_clear(MZn);
    gr_ctx_clear(Zn);

    gr_ctx_init_real_qqbar(QQbar_real);
    gr_test_ring(QQbar_real, 100);
    gr_ctx_clear(QQbar_real);

    gr_ctx_init_complex_qqbar(QQbar);
    gr_test_ring(QQbar, 100);
    gr_ctx_clear(QQbar);

    flint_cleanup();
    return 0;
}

