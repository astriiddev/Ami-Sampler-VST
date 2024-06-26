#include "ami_font.h"

const unsigned char font_exmrk[8] = { 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00 };
const unsigned char font_quote[8] = { 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30 };
const unsigned char font_nmsgn[8] = { 0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00 };
const unsigned char font_dllar[8] = { 0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00 };
const unsigned char font_prcnt[8] = { 0x00, 0x66, 0xAC, 0xD8, 0x36, 0x6A, 0xCC, 0x00 };
const unsigned char font_ampsd[8] = { 0x38, 0x6C, 0x68, 0x76, 0xDC, 0xCE, 0x7F, 0x00 };
const unsigned char font_apstf[8] = { 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char font_lparn[8] = { 0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00 };
const unsigned char font_rparn[8] = { 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00 };
const unsigned char font_astrx[8] = { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00 };
const unsigned char font_plssn[8] = { 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00 };
const unsigned char font_comma[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30 };
const unsigned char font_hyphn[8] = { 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00 };
const unsigned char font_priod[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 };
const unsigned char font_fslsh[8] = { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x00 };

const unsigned char font_0[8] = { 0x3C, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x3C, 0x00 };
const unsigned char font_1[8] = { 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_2[8] = { 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00 };
const unsigned char font_3[8] = { 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00 };
const unsigned char font_4[8] = { 0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x00 };
const unsigned char font_5[8] = { 0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00 };
const unsigned char font_6[8] = { 0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00 };
const unsigned char font_7[8] = { 0x7E, 0x06, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_8[8] = { 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00 };
const unsigned char font_9[8] = { 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00 };

const unsigned char font_colon[8] = { 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00 };
const unsigned char font_scoln[8] = { 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30 };
const unsigned char font_lthan[8] = { 0x00, 0x06, 0x18, 0x60, 0x18, 0x06, 0x00, 0x00 };
const unsigned char font_equal[8] = { 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00 };
const unsigned char font_gthan[8] = { 0x00, 0x60, 0x18, 0x06, 0x18, 0x60, 0x00, 0x00 };
const unsigned char font_qmark[8] = { 0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00 };
const unsigned char font_atsym[8] = { 0x7C, 0xC6, 0xDE, 0xD6, 0xDE, 0xC0, 0x78, 0x00 };

const unsigned char font_A[8] = { 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00 };
const unsigned char font_B[8] = { 0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00 };
const unsigned char font_C[8] = { 0x1E, 0x30, 0x60, 0x60, 0x60, 0x30, 0x1E, 0x00 };
const unsigned char font_D[8] = { 0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00 };
const unsigned char font_E[8] = { 0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00 };
const unsigned char font_F[8] = { 0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00 };
const unsigned char font_G[8] = { 0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3E, 0x00 };
const unsigned char font_H[8] = { 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00 };
const unsigned char font_I[8] = { 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00 };
const unsigned char font_J[8] = { 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C, 0x00 };
const unsigned char font_K[8] = { 0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00 };
const unsigned char font_L[8] = { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00 };
const unsigned char font_M[8] = { 0xC6, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0x00 };
const unsigned char font_N[8] = { 0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00 };
const unsigned char font_O[8] = { 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00 };
const unsigned char font_P[8] = { 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00 };
const unsigned char font_Q[8] = { 0x78, 0xCC, 0xCC, 0xCC, 0xCC, 0xDC, 0x7E, 0x00 };
const unsigned char font_R[8] = { 0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00 };
const unsigned char font_S[8] = { 0x3C, 0x66, 0x70, 0x3C, 0x0E, 0x66, 0x3C, 0x00 };
const unsigned char font_T[8] = { 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_U[8] = { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00 };
const unsigned char font_V[8] = { 0x66, 0x66, 0x66, 0x66, 0x3C, 0x3C, 0x18, 0x00 };
const unsigned char font_W[8] = { 0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00 };
const unsigned char font_X[8] = { 0xC3, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0xC3, 0x00 };
const unsigned char font_Y[8] = { 0xC3, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_Z[8] = { 0xFE, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xFE, 0x00 };

const unsigned char font_lbrck[8] = { 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00 };
const unsigned char font_bslsh[8] = { 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x00 };
const unsigned char font_rbrck[8] = { 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00 };
const unsigned char font_carrt[8] = { 0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00 };
const unsigned char font_uline[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE };
const unsigned char font_accnt[8] = { 0x18, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char font_a[8] = { 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00 };
const unsigned char font_b[8] = { 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00 };
const unsigned char font_c[8] = { 0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00 };
const unsigned char font_d[8] = { 0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00 };
const unsigned char font_e[8] = { 0x00, 0x00, 0x3C, 0x66, 0x7C, 0x60, 0x3C, 0x00 };
const unsigned char font_f[8] = { 0x1C, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x30, 0x00 };
const unsigned char font_g[8] = { 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C };
const unsigned char font_h[8] = { 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00 };
const unsigned char font_i[8] = { 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_j[8] = { 0x06, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3C };
const unsigned char font_k[8] = { 0x60, 0x60, 0x66, 0x6C, 0x7C, 0x6C, 0x66, 0x00 };
const unsigned char font_l[8] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00 };
const unsigned char font_m[8] = { 0x00, 0x00, 0xEC, 0xFE, 0xD6, 0xC6, 0xC6, 0x00 };
const unsigned char font_n[8] = { 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00 };
const unsigned char font_o[8] = { 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00 };
const unsigned char font_p[8] = { 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60 };
const unsigned char font_q[8] = { 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06 };
const unsigned char font_r[8] = { 0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00 };
const unsigned char font_s[8] = { 0x00, 0x00, 0x3C, 0x60, 0x3C, 0x0E, 0x7C, 0x00 };
const unsigned char font_t[8] = { 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x3C, 0x00 };
const unsigned char font_u[8] = { 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00 };
const unsigned char font_v[8] = { 0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00 };
const unsigned char font_w[8] = { 0x00, 0x00, 0xC6, 0xC6, 0xD6, 0xFE, 0x6C, 0x00 };
const unsigned char font_x[8] = { 0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00 };
const unsigned char font_y[8] = { 0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x30 };
const unsigned char font_z[8] = { 0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00 };

const unsigned char font_lbrce[8] = { 0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00 };
const unsigned char font_vline[8] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 };
const unsigned char font_rbrce[8] = { 0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00 };
const unsigned char font_tilde[8] = { 0x71, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char* font[94] = 
{ 
        font_exmrk, font_quote, font_nmsgn, font_dllar, font_prcnt, font_ampsd, font_apstf, font_lparn, 
        font_rparn, font_astrx, font_plssn, font_comma, font_hyphn, font_priod, font_fslsh,
                   
        font_0, font_1, font_2, font_3, font_4, font_5, font_6, font_7, font_8, font_9,
     
        font_colon, font_scoln, font_lthan, font_equal, font_gthan, font_qmark, font_atsym,

        font_A, font_B, font_C, font_D, font_E, font_F, font_G, font_H, font_I, font_J, font_K, font_L, font_M, 
        font_N, font_O, font_P, font_Q, font_R, font_S, font_T, font_U, font_V, font_W, font_X, font_Y, font_Z,

        font_lbrck, font_bslsh, font_rbrck, font_carrt, font_uline, font_accnt,
                   
        font_a, font_b, font_c, font_d, font_e, font_f, font_g, font_h, font_i, font_j, font_k, font_l, font_m,
        font_n, font_o, font_p, font_q, font_r, font_s, font_t, font_u, font_v, font_w, font_x, font_y, font_z,

        font_lbrce, font_vline, font_rbrce, font_tilde
};

const unsigned char& getAmiFont(int letter, int bit) { return font[letter][bit]; }
