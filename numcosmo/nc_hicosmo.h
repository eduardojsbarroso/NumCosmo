/***************************************************************************
 *            nc_hicosmo.h
 *
 *  Mon Jul 16 18:03:42 2007
 *  Copyright  2007  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * numcosmo
 * Copyright (C) Sandro Dias Pinto Vitenti 2012 <sandro@isoftware.com.br>
 * numcosmo is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * numcosmo is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NC_HICOSMO_H_
#define _NC_HICOSMO_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define NC_TYPE_MODEL             (nc_hicosmo_get_type ())
#define NC_HICOSMO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), NC_TYPE_MODEL, NcHICosmo))
#define NC_HICOSMO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), NC_TYPE_MODEL, NcHICosmoClass))
#define NC_IS_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NC_TYPE_MODEL))
#define NC_IS_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), NC_TYPE_MODEL))
#define NC_HICOSMO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), NC_TYPE_MODEL, NcHICosmoClass))

/**
 * NcHICosmoImpl:
 * @NC_HICOSMO_IMPL_H0: FIXME
 * @NC_HICOSMO_IMPL_Omega_b: FIXME
 * @NC_HICOSMO_IMPL_Omega_r: FIXME
 * @NC_HICOSMO_IMPL_Omega_c: FIXME
 * @NC_HICOSMO_IMPL_Omega_t: FIXME
 * @NC_HICOSMO_IMPL_sigma_8: FIXME
 * @NC_HICOSMO_IMPL_T_gamma0: FIXME
 * @NC_HICOSMO_IMPL_z_lss: FIXME
 * @NC_HICOSMO_IMPL_E2: FIXME
 * @NC_HICOSMO_IMPL_dE2_dz: FIXME
 * @NC_HICOSMO_IMPL_d2E2_dz2: FIXME
 * @NC_HICOSMO_IMPL_cd: FIXME
 * @NC_HICOSMO_IMPL_powspec: FIXME
 *
 * FIXME
 */
typedef enum _NcHICosmoImpl
{
  NC_HICOSMO_IMPL_H0        = 1 << 0,
  NC_HICOSMO_IMPL_Omega_b   = 1 << 1,
  NC_HICOSMO_IMPL_Omega_r   = 1 << 2,
  NC_HICOSMO_IMPL_Omega_c   = 1 << 3,
  NC_HICOSMO_IMPL_Omega_t   = 1 << 4,
  NC_HICOSMO_IMPL_sigma_8   = 1 << 5,
  NC_HICOSMO_IMPL_T_gamma0  = 1 << 6,
  NC_HICOSMO_IMPL_z_lss     = 1 << 7,
  NC_HICOSMO_IMPL_E2        = 1 << 8,
  NC_HICOSMO_IMPL_dE2_dz    = 1 << 9,
  NC_HICOSMO_IMPL_d2E2_dz2  = 1 << 10,
  NC_HICOSMO_IMPL_cd        = 1 << 11,
  NC_HICOSMO_IMPL_powspec   = 1 << 12, /*< private >*/
  NC_HICOSMO_IMPL_LAST      = 1 << 13, /*< skip >*/
} NcHICosmoImpl;

typedef struct _NcHICosmoClass NcHICosmoClass;
typedef struct _NcHICosmo NcHICosmo;
typedef gdouble (*NcHICosmoFunc0) (NcHICosmo *model);
typedef gdouble (*NcHICosmoFunc1) (NcHICosmo *model, gdouble x);

struct _NcHICosmoClass
{
  /*< private >*/
  NcmModelClass parent_class;
  NcmModelFunc0 H0;
  NcmModelFunc0 Omega_b;
  NcmModelFunc0 Omega_r;
  NcmModelFunc0 Omega_c;
  NcmModelFunc0 Omega_t;
  NcmModelFunc0 sigma_8;
  NcmModelFunc0 T_gamma0;
  NcmModelFunc0 z_lss;
  NcmModelFunc1 E2;
  NcmModelFunc1 dE2_dz;
  NcmModelFunc1 d2E2_dz2;
  NcmModelFunc1 cd;
  NcmModelFunc1 powspec;
};

/**
 * NcHICosmo:
 *
 * FIXME
 */
struct _NcHICosmo
{
  /*< private >*/
  NcmModel parent_instance;
  gboolean is_eternal;
};

extern gint32 NC_HICOSMO_ID;

GType nc_hicosmo_get_type (void) G_GNUC_CONST;

/*
 * Cosmological model constant functions
 */
G_INLINE_FUNC gdouble nc_hicosmo_H0 (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_b (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_r (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_c (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_t (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_T_gamma0 (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_sigma_8 (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_z_lss (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_E2 (NcHICosmo *model, gdouble x);
G_INLINE_FUNC gdouble nc_hicosmo_dE2_dz (NcHICosmo *model, gdouble x);
G_INLINE_FUNC gdouble nc_hicosmo_d2E2_dz2 (NcHICosmo *model, gdouble x);
G_INLINE_FUNC gdouble nc_hicosmo_cd (NcHICosmo *model, gdouble x);
G_INLINE_FUNC gdouble nc_hicosmo_powspec (NcHICosmo *model, gdouble x);
G_INLINE_FUNC gdouble nc_hicosmo_c_H0 (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_k (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_Omega_m (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_h (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_h2 (NcHICosmo *model);
G_INLINE_FUNC gdouble nc_hicosmo_E (NcHICosmo *model, gdouble z);
G_INLINE_FUNC gdouble nc_hicosmo_H (NcHICosmo *model, gdouble z);
G_INLINE_FUNC gdouble nc_hicosmo_dH_dz (NcHICosmo *model, gdouble z);
G_INLINE_FUNC gdouble nc_hicosmo_j (NcHICosmo *model, gdouble z);
G_INLINE_FUNC gdouble nc_hicosmo_qp (NcHICosmo *model, gdouble z);
G_INLINE_FUNC gdouble nc_hicosmo_q (NcHICosmo *model, gdouble z);

NcHICosmo *nc_hicosmo_new_from_name (GType parent_type, gchar *model_name);
void nc_hicosmo_log_all_models (GType parent);
void nc_hicosmo_free (NcHICosmo *hic);

NcmMSetFunc *nc_hicosmo_func0_new (NcHICosmoFunc0 f0);
NcmMSetFunc *nc_hicosmo_func1_new (NcHICosmoFunc1 f1);

void nc_hicosmo_set_H0_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_Omega_b_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_Omega_r_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_Omega_c_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_Omega_t_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_sigma_8_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);
void nc_hicosmo_set_T_gamma0_impl (NcHICosmoClass *model_class, NcmModelFunc0 f);

void nc_hicosmo_set_E2_impl (NcHICosmoClass *model_class, NcmModelFunc1 f);
void nc_hicosmo_set_dE2_dz_impl (NcHICosmoClass *model_class, NcmModelFunc1 f);
void nc_hicosmo_set_d2E2_dz2_impl (NcHICosmoClass *model_class, NcmModelFunc1 f);
void nc_hicosmo_set_cd_impl (NcHICosmoClass *model_class, NcmModelFunc1 f);
void nc_hicosmo_set_powspec_impl (NcHICosmoClass *model_class, NcmModelFunc1 f);

#define NC_HICOSMO_DEFAULT_PARAMS_RELTOL (1e-7)
#define NC_HICOSMO_DEFAULT_PARAMS_ABSTOL (0.0)

G_END_DECLS

#endif /* _NC_HICOSMO_H_ */

#ifndef _NC_HICOSMO_INLINE_H_
#define _NC_HICOSMO_INLINE_H_
#ifdef NUMCOSMO_HAVE_INLINE

G_BEGIN_DECLS

NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,H0)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,Omega_b)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,Omega_r)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,Omega_c)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,Omega_t)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,T_gamma0)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,sigma_8)
NCM_MODEL_FUNC0_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,z_lss)

NCM_MODEL_FUNC1_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,E2)
NCM_MODEL_FUNC1_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,dE2_dz)
NCM_MODEL_FUNC1_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,d2E2_dz2)
NCM_MODEL_FUNC1_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,cd)
NCM_MODEL_FUNC1_IMPL (NC_HICOSMO,NcHICosmo,nc_hicosmo,powspec)

G_INLINE_FUNC gdouble
nc_hicosmo_c_H0 (NcHICosmo *model)
{
  return (NC_C_c / nc_hicosmo_H0 (model));
}

G_INLINE_FUNC gdouble
nc_hicosmo_Omega_k (NcHICosmo *model)
{
  return (1.0 - nc_hicosmo_Omega_t (model));
}

G_INLINE_FUNC gdouble
nc_hicosmo_Omega_m (NcHICosmo *model)
{
  return (nc_hicosmo_Omega_b (model) + nc_hicosmo_Omega_c (model));
}

G_INLINE_FUNC gdouble
nc_hicosmo_H (NcHICosmo *model, gdouble z)
{
  return (nc_hicosmo_H0 (model) * sqrt (nc_hicosmo_E2 (model, z)));
}

G_INLINE_FUNC gdouble
nc_hicosmo_h (NcHICosmo *model)
{
  return nc_hicosmo_H0 (model) / 100.0;
}

G_INLINE_FUNC gdouble
nc_hicosmo_h2 (NcHICosmo *model)
{
  return gsl_pow_2 (nc_hicosmo_H0 (model) / 100.0);
}

G_INLINE_FUNC gdouble
nc_hicosmo_E (NcHICosmo *model, gdouble z)
{
  return sqrt(nc_hicosmo_E2 (model, z));
}

G_INLINE_FUNC gdouble
nc_hicosmo_dH_dz (NcHICosmo *model, gdouble z)
{
  return nc_hicosmo_H0 (model) *
	nc_hicosmo_dE2_dz (model, z) / (2.0 * sqrt (nc_hicosmo_E2 (model, z)));
}

G_INLINE_FUNC gdouble
nc_hicosmo_q (NcHICosmo *model, gdouble z)
{
  gdouble E2, dE2_dz;
  E2 = nc_hicosmo_E2 (model, z);
  dE2_dz = nc_hicosmo_dE2_dz (model, z);
  return (dE2_dz * (1.0 + z) / (2.0 * E2) - 1.0);
}

G_INLINE_FUNC gdouble
nc_hicosmo_qp (NcHICosmo *model, gdouble z)
{
  gdouble E2, dE2_dz, d2E2_dz2;
  E2 = nc_hicosmo_E2 (model, z);
  dE2_dz = nc_hicosmo_dE2_dz (model, z);
  d2E2_dz2 = nc_hicosmo_d2E2_dz2 (model, z);

  return (1.0 + z) / (2.0 * E2) * (dE2_dz / (1.0 + z) + d2E2_dz2 - dE2_dz * dE2_dz / E2);
}

G_INLINE_FUNC gdouble
nc_hicosmo_j (NcHICosmo *model, gdouble z)
{
  gdouble E2, dE2_dz, d2E2_dz2;
  E2 = nc_hicosmo_E2 (model, z);
  dE2_dz = nc_hicosmo_dE2_dz (model, z);
  d2E2_dz2 = nc_hicosmo_d2E2_dz2 (model, z);

  return gsl_pow_2 (1.0 + z) * (d2E2_dz2 - 2.0 * dE2_dz / (1.0 + z)) / (2.0 * E2) + 1.0;
}

G_END_DECLS

#endif /* NUMCOSMO_HAVE_INLINE */
#endif /* _NC_HICOSMO_INLINE_H_ */
