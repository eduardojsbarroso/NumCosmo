/***************************************************************************
 *            test_nc_ccl_Pk.c
 *
 *  Mon February 25 16:43:00 2019
 *  Copyright  2019  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * test_nc_ccl_Pk.c
 *
 * Copyright (C) 2019 - Sandro Dias Pinto Vitenti
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#undef GSL_RANGE_CHECK_OFF
#endif /* HAVE_CONFIG_H */
#include <numcosmo/numcosmo.h>

#include <math.h>
#include <glib.h>
#include <glib-object.h>
#include <ccl.h>

typedef struct _TestNcCCLPk
{
  ccl_cosmology * ccl_cosmo;
  NcHICosmo *cosmo;
  NcGrowthFunc *gf;
  NcPowspecML *Pk; 
} TestNcCCLPk;

void test_nc_ccl_pk_new_model1_bbks (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model2_bbks (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model3_bbks (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model1_eh (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model2_eh (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model3_eh (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model1_cbe (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model2_cbe (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_new_model3_cbe (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_free (TestNcCCLPk *test, gconstpointer pdata);

void test_nc_ccl_pk_cmp (TestNcCCLPk *test, gconstpointer pdata);

void test_nc_ccl_pk_bbks_traps (TestNcCCLPk *test, gconstpointer pdata);
void test_nc_ccl_pk_bbks_invalid_st (TestNcCCLPk *test, gconstpointer pdata);

#define BBKS_TOLERANCE 1.0E-5
#define CTEST_DATA(name) typedef struct _##name##_data name##_data; struct _##name##_data
#define CTEST_SETUP(name) void name##_setup (name##_data* data)

typedef struct _ccl_params_data 
{
  double Omega_c;
  double Omega_b;
  double h;
  double A_s;
  double n_s;
  double sigma8;
  double Neff;
  double mnu;
  ccl_mnu_convention mnu_type;
  double Omega_v[5];
  double Omega_k[5];
  double w_0[5];
  double w_a[5];
} ccl_params_data;

void
ccl_params_data_init (ccl_params_data *data)
{
  data->Omega_c  = 0.25;
  data->Omega_b  = 0.05;
  data->h        = 0.7;
  data->A_s      = 2.1e-9;
  data->n_s      = 0.96;
  data->sigma8   = 0.8;
  data->Neff     = 3.046;
  data->mnu      = 0.0;
  data->mnu_type = ccl_mnu_sum;

  double Omega_v[5] = { 0.7,  0.7,  0.7,  0.65, 0.75};
  double w_0[5]     = {-1.0, -0.9, -0.9, -0.9, -0.9};
  double w_a[5]     = { 0.0,  0.0,  0.1,  0.1,  0.1};

  for (int i=0;i<5;i++) 
  {
    data->Omega_v[i] = Omega_v[i];
    data->w_0[i]     = w_0[i];
    data->w_a[i]     = w_a[i];
    data->Omega_k[i] = 1.0 - data->Omega_c - data->Omega_b - data->Omega_v[i];
  }
}

gint
main (gint argc, gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  ncm_cfg_init_full_ptr (&argc, &argv);
  ncm_cfg_enable_gsl_err_handler ();

  g_test_set_nonfatal_assertions ();
  
  g_test_add ("/nc/ccl/Pk/model1/bbks/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model1_bbks,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model2/bbks/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model2_bbks,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model3/bbks/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model3_bbks,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model1/eh/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model1_eh,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model2/eh/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model2_eh,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model3/eh/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model3_eh,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model1/cbe/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model1_cbe,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model2/cbe/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model2_cbe,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model3/cbe/cmp", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model3_cbe,
              &test_nc_ccl_pk_cmp,
              &test_nc_ccl_pk_free);

  g_test_add ("/nc/ccl/Pk/model1/bbks/traps", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model1_bbks,
              &test_nc_ccl_pk_bbks_traps,
              &test_nc_ccl_pk_free);

#if GLIB_CHECK_VERSION (2, 38, 0)
  g_test_add ("/nc/ccl/Pk/model1/bbks/invalid/st/subprocess", TestNcCCLPk, NULL,
              &test_nc_ccl_pk_new_model1_bbks,
              &test_nc_ccl_pk_bbks_invalid_st,
              &test_nc_ccl_pk_free);
#endif
  g_test_run ();
}

ccl_cosmology *
test_nc_create_ccl_cosmo (gint i_model, transfer_function_t pk_t)
{
  ccl_params_data *data    = g_new0 (ccl_params_data, 1);
  ccl_configuration config = default_config;
  gint status              = 0;
  ccl_parameters params;
  ccl_cosmology *cosmo;

  ccl_params_data_init (data);
  config.transfer_function_method = pk_t;

  params = ccl_parameters_create (data->Omega_c, data->Omega_b, data->Omega_k[i_model - 1], data->Neff, &data->mnu, data->mnu_type, data->w_0[i_model - 1], data->w_a[i_model - 1], data->h, data->sigma8, data->n_s, -1, -1, -1, -1, NULL, NULL, &status);
  /*params.Omega_g = 0;*/
  /*printf ("# Setting % 22.15g % 22.15g\n", params.Omega_l, data->Omega_v[i_model - 1]);*/
  /*params.Omega_l = data->Omega_v[i_model - 1];*/
  /*params.sigma8  = data->sigma8;*/

  cosmo = ccl_cosmology_create (params, config);

  /*cosmo->spline_params.A_SPLINE_NLOG_PK = 200;*/
  /*cosmo->spline_params.A_SPLINE_NA_PK   = 200;*/
  /*cosmo->spline_params.N_K              = 100;*/
	/*cosmo->spline_params.K_MAX_SPLINE     = 1.0e3;*/

  g_free (data);

  return cosmo;
}

void 
test_nc_ccl_pk_create_BBKS (TestNcCCLPk *test, gconstpointer pdata)
{
  NcTransferFunc *tf = nc_transfer_func_bbks_new ();

  test->Pk = NC_POWSPEC_ML (nc_powspec_ml_transfer_new (tf));
  test->gf = nc_powspec_ml_transfer_peek_gf (NC_POWSPEC_ML_TRANSFER (test->Pk));

  nc_transfer_func_bbks_set_type (NC_TRANSFER_FUNC_BBKS (tf), NC_TRANSFER_FUNC_BBKS_TYPE_BARYONS);

  ncm_powspec_require_zi (NCM_POWSPEC (test->Pk), 0.0);
  ncm_powspec_require_zf (NCM_POWSPEC (test->Pk), 6.0);
  ncm_powspec_require_kmin (NCM_POWSPEC (test->Pk), 1.0e-3);
  ncm_powspec_require_kmax (NCM_POWSPEC (test->Pk), 1.0e+1);

  ncm_powspec_prepare (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo));

  {
    NcmModel *mprim = ncm_model_peek_submodel_by_mid (NCM_MODEL (test->cosmo), nc_hiprim_id ());
    ncm_model_param_set_by_name (mprim, "ln10e10ASA", 
                                 ncm_model_param_get_by_name (mprim, "ln10e10ASA") 
                                 + 2.0 * log (test->ccl_cosmo->params.sigma8 / nc_powspec_ml_sigma_R (test->Pk, NCM_MODEL (test->cosmo), 1.0e-7, 0.0, 8.0 / nc_hicosmo_h (test->cosmo))));
  }

  ncm_powspec_prepare (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo));
  nc_transfer_func_free (tf);
}

void 
test_nc_ccl_pk_create_EH (TestNcCCLPk *test, gconstpointer pdata)
{
  NcTransferFunc *tf = nc_transfer_func_eh_new ();

  nc_transfer_func_eh_set_CCL_comp (NC_TRANSFER_FUNC_EH (tf), TRUE);
  
  test->Pk = NC_POWSPEC_ML (nc_powspec_ml_transfer_new (tf));
  test->gf = nc_powspec_ml_transfer_peek_gf (NC_POWSPEC_ML_TRANSFER (test->Pk));

  ncm_powspec_require_zi (NCM_POWSPEC (test->Pk), 0.0);
  ncm_powspec_require_zf (NCM_POWSPEC (test->Pk), 6.0);
  ncm_powspec_require_kmin (NCM_POWSPEC (test->Pk), 1.0e-3);
  ncm_powspec_require_kmax (NCM_POWSPEC (test->Pk), 1.0e+1);

  ncm_powspec_prepare (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo));
  
  {
    NcmModel *mprim = ncm_model_peek_submodel_by_mid (NCM_MODEL (test->cosmo), nc_hiprim_id ());
    ncm_model_param_set_by_name (mprim, "ln10e10ASA", ncm_model_param_get_by_name (mprim, "ln10e10ASA") 
                                 + 2.0 * log (test->ccl_cosmo->params.sigma8 / nc_powspec_ml_sigma_R (test->Pk, NCM_MODEL (test->cosmo), 1.0e-7, 0.0, 8.0 / nc_hicosmo_h (test->cosmo))));
  }
  
  ncm_powspec_prepare (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo));
  
  nc_transfer_func_free (tf);
}

void 
test_nc_ccl_pk_create_cbe (TestNcCCLPk *test, gconstpointer pdata)
{
  NcCBE *cbe;

  test->Pk = NC_POWSPEC_ML (nc_powspec_ml_cbe_new ());
  cbe      = nc_powspec_ml_cbe_peek_cbe (NC_POWSPEC_ML_CBE (test->Pk));

	nc_powspec_ml_cbe_set_intern_k_min (NC_POWSPEC_ML_CBE (test->Pk), test->ccl_cosmo->spline_params.K_MIN);
	nc_powspec_ml_cbe_set_intern_k_max (NC_POWSPEC_ML_CBE (test->Pk), test->ccl_cosmo->spline_params.K_MAX_SPLINE);
  nc_cbe_use_ppf (cbe, TRUE);
  /*g_object_set (cbe, "verbosity", 1, NULL);*/

  ncm_powspec_require_zi (NCM_POWSPEC (test->Pk), 0.0);
  ncm_powspec_require_zf (NCM_POWSPEC (test->Pk), 6.0);
  ncm_powspec_require_kmin (NCM_POWSPEC (test->Pk), 1.0e-3);
  ncm_powspec_require_kmax (NCM_POWSPEC (test->Pk), 1.0e+1);

  ncm_powspec_prepare (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo));

  if (FALSE)
  {
    ncm_model_orig_params_log_all (NCM_MODEL (test->cosmo));
    ncm_model_orig_params_log_all (ncm_model_peek_submodel_by_mid (NCM_MODEL (test->cosmo), nc_hiprim_id ()));
    ncm_model_orig_params_log_all (ncm_model_peek_submodel_by_mid (NCM_MODEL (test->cosmo), nc_hireion_id ()));
    printf ("# As: % 22.15g, sigma8_cbe: % 22.15g, sigma8_int: % 22.15g\n", 
            nc_hiprim_SA_Ampl (NC_HIPRIM (ncm_model_peek_submodel_by_mid (NCM_MODEL (test->cosmo), nc_hiprim_id ()))),
            nc_cbe_get_sigma8 (cbe),
            nc_powspec_ml_sigma_R (test->Pk, NCM_MODEL (test->cosmo), 1.0e-7, 0.0, 8.0 / nc_hicosmo_h (test->cosmo))
            );
  }

}

void
test_nc_ccl_pk_new_model1_bbks (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (1, ccl_bbks);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_BBKS (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model2_bbks (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (2, ccl_bbks);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_BBKS (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model3_bbks (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (3, ccl_bbks);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_BBKS (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model1_eh (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (1, ccl_eisenstein_hu);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_EH (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model2_eh (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (2, ccl_eisenstein_hu);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_EH (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model3_eh (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (3, ccl_eisenstein_hu);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_EH (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model1_cbe (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (1, ccl_boltzmann_class);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_cbe (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model2_cbe (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (2, ccl_boltzmann_class);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_cbe (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_new_model3_cbe (TestNcCCLPk *test, gconstpointer pdata)
{
  test->ccl_cosmo = test_nc_create_ccl_cosmo (3, ccl_boltzmann_class);
  test->cosmo     = NC_HICOSMO (nc_hicosmo_de_cpl_new_from_ccl (&test->ccl_cosmo->params));

  /*ncm_model_param_set_by_name (NCM_MODEL (test->cosmo), "Tgamma0", 0.0);*/

  test_nc_ccl_pk_create_cbe (test, pdata);
  
  g_assert (NC_IS_HICOSMO_DE_CPL (test->cosmo));
}

void
test_nc_ccl_pk_free (TestNcCCLPk *test, gconstpointer pdata)
{
  NCM_TEST_FREE (nc_hicosmo_free, test->cosmo);
  NCM_TEST_FREE (nc_powspec_ml_free, test->Pk);

  ccl_parameters_free (&test->ccl_cosmo->params);
  ccl_cosmology_free (test->ccl_cosmo);  
}

void
test_nc_ccl_pk_cmp (TestNcCCLPk *test, gconstpointer pdata)
{
  const gint ntests       = 200;
  const gdouble z_max     = 6.0;
  const gdouble tol       = 5.0e-5;
  const gdouble tol_Pk    = 6.0e-5;
  gint status = 0;
  gint i;

  
  for (i = 0; i < ntests; i++)
  {
    const gdouble z     = z_max / (1.0 * ntests) * i;
    const gdouble a     = 1.0 / (1.0 + z);
    const gdouble E2    = nc_hicosmo_E2 (test->cosmo, z);
    const gdouble cclE2 = gsl_pow_2 (ccl_h_over_h0 (test->ccl_cosmo, a, &status));
    gint j;

    if (TRUE)
    {
      const gdouble E2Omega_x    = nc_hicosmo_de_E2Omega_de (NC_HICOSMO_DE (test->cosmo), z);
      const gdouble cclE2Omega_x = ccl_omega_x (test->ccl_cosmo, a, ccl_species_l_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_x, ==, cclE2Omega_x, tol, 0.0);
      
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_X: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_x, cclE2Omega_x, E2Omega_x / cclE2Omega_x - 1.0);*/ 
    }

    if (TRUE)
    {
      const gdouble E2Omega_mnu    = nc_hicosmo_E2Omega_mnu (NC_HICOSMO (test->cosmo), z);
      const gdouble cclE2Omega_mnu = ccl_omega_x (test->ccl_cosmo, a, ccl_species_nu_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_mnu, ==, cclE2Omega_mnu, tol, 0.0);
      
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_MNU: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_mnu, cclE2Omega_mnu, E2Omega_mnu / cclE2Omega_mnu - 1.0);*/ 
    }

    if (TRUE)
    {
      const gdouble E2Omega_m    = nc_hicosmo_E2Omega_m (test->cosmo, z);
      const gdouble cclE2Omega_m = ccl_omega_x (test->ccl_cosmo, a, ccl_species_m_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_m, ==, cclE2Omega_m, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_M: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_m, cclE2Omega_m, E2Omega_m / cclE2Omega_m - 1.0);*/ 
    }

    if (TRUE)
    {
      const gdouble E2Omega_g    = nc_hicosmo_E2Omega_g (test->cosmo, z);
      const gdouble cclE2Omega_g = ccl_omega_x (test->ccl_cosmo, a, ccl_species_g_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_g, ==, cclE2Omega_g, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_G: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_g, cclE2Omega_g, E2Omega_g / cclE2Omega_g - 1.0);*/    
    }

    if (TRUE)
    {
      const gdouble E2Omega_nu    = nc_hicosmo_E2Omega_nu (test->cosmo, z);
      const gdouble cclE2Omega_nu = ccl_omega_x (test->ccl_cosmo, a, ccl_species_ur_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_nu, ==, cclE2Omega_nu, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_NU: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_nu, cclE2Omega_nu, E2Omega_nu / cclE2Omega_nu - 1.0);*/    
    }
    
    if (TRUE)
    {
      const gdouble E2Omega_k    = nc_hicosmo_E2Omega_k (test->cosmo, z);
      const gdouble cclE2Omega_k = ccl_omega_x (test->ccl_cosmo, a, ccl_species_k_label, &status) * cclE2;

      ncm_assert_cmpdouble_e (E2Omega_k, ==, cclE2Omega_k, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | OMEGA_K: % 22.15g % 22.15g %17.10e;\n", z, a, E2Omega_k, cclE2Omega_k, E2Omega_k / cclE2Omega_k - 1.0);*/     
    }

    if (TRUE)
    {
      ncm_assert_cmpdouble_e (E2, ==, cclE2, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | E2 % 22.15g % 22.15g %17.10e\n", z, a, E2, cclE2, E2 / cclE2 - 1.0);*/ 
    }

    if (test->gf != NULL)
    {
      const gdouble D    = nc_growth_func_eval (test->gf, test->cosmo, z);
      const gdouble cclD = ccl_growth_factor (test->ccl_cosmo, a, &status);

      ncm_assert_cmpdouble_e (D, ==, cclD, tol, 0.0);
      /*printf ("(% 22.15g, % 22.15g) | D % 22.15g % 22.15g %17.10e\n", z, a, D, cclD, D / cclD - 1.0);*/
    }

    if (TRUE)
    {    
      for (j = 0; j < ntests; j++)
      {
        const gdouble k      = exp (log (1.0e-3) + log (1.0e4) * j / (ntests - 1.0));
        const gdouble Pkz    = ncm_powspec_eval (NCM_POWSPEC (test->Pk), NCM_MODEL (test->cosmo), z, k);
        const gdouble cclPkz = ccl_linear_matter_power (test->ccl_cosmo, k, 1.0 / (1.0 + z), &status);

        ncm_assert_cmpdouble_e (Pkz, ==, cclPkz, tol_Pk, 0.0);
        /*printf ("% 22.15g % 22.15g | % 22.15g % 22.15g % 22.15g %17.10e\n", k, z, Pkz, cclPkz, Pkz / cclPkz, Pkz / cclPkz - 1.0);*/
      }
    }
  }
}

void
test_nc_ccl_pk_bbks_traps (TestNcCCLPk *test, gconstpointer pdata)
{
#if GLIB_CHECK_VERSION(2,38,0)
  g_test_trap_subprocess ("/nc/ccl/Pk/model1/bbks/invalid/st/subprocess", 0, 0);
  g_test_trap_assert_failed ();
#endif
}

void
test_nc_ccl_pk_bbks_invalid_st (TestNcCCLPk *test, gconstpointer pdata)
{
  g_assert_not_reached ();
}
