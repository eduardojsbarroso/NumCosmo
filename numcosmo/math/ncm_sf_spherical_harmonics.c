/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*-  */
/***************************************************************************
 *            ncm_sf_spherical_harmonics.c
 *
 *  Thu December 14 11:18:00 2017
 *  Copyright  2017  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * ncm_sf_spherical_harmonics.c
 * Copyright (C) 2017 Sandro Dias Pinto Vitenti <sandro@isoftware.com.br>
 *
 * NumCosmo is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * NumCosmo is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:ncm_sf_spherical_harmonics
 * @title: NcmSFSphericalHarmonics
 * @short_description: Spherical Harmonics object
 *
 * FIXME
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */
#include "build_cfg.h"

#include "math/ncm_sf_spherical_harmonics.h"

#ifndef NUMCOSMO_GIR_SCAN
#include <math.h>
#endif /* NUMCOSMO_GIR_SCAN */

enum
{
	PROP_0,
	PROP_LMAX,
	PROP_ABSTOL,
};

G_DEFINE_TYPE (NcmSFSphericalHarmonics, ncm_sf_spherical_harmonics, G_TYPE_OBJECT);

static void
ncm_sf_spherical_harmonics_init (NcmSFSphericalHarmonics *spha)
{
	spha->lmax     = 0;
	spha->l        = 0;
	spha->l0       = 0;
	spha->m        = 0;
	spha->Klm      = NULL;
	spha->sqrt_n   = g_array_new (FALSE, FALSE, sizeof (gdouble));
	spha->sqrtm1_n = g_array_new (FALSE, FALSE, sizeof (gdouble));
	spha->K_array  = g_ptr_array_new ();
	spha->x        = 0.0;
	spha->sqrt1mx2 = 0.0;
	spha->Pl0m     = 0.0;
	spha->Plm      = 0.0;
	spha->Plp1m    = 0.0;
	spha->abstol   = 0.0;

	g_ptr_array_set_free_func (spha->K_array, (GDestroyNotify) g_array_unref);
}

static void
_ncm_sf_spherical_harmonics_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	NcmSFSphericalHarmonics *spha = NCM_SF_SPHERICAL_HARMONICS (object);
	g_return_if_fail (NCM_IS_SF_SPHERICAL_HARMONICS (object));

	switch (prop_id)
	{
		case PROP_LMAX:
			ncm_sf_spherical_harmonics_set_lmax (spha, g_value_get_int (value));
			break;
		case PROP_ABSTOL:
			ncm_sf_spherical_harmonics_set_abstol (spha, g_value_get_double (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
_ncm_sf_spherical_harmonics_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	NcmSFSphericalHarmonics *spha = NCM_SF_SPHERICAL_HARMONICS (object);
	g_return_if_fail (NCM_IS_SF_SPHERICAL_HARMONICS (object));

	switch (prop_id)
	{
		case PROP_LMAX:
			g_value_set_int (value, ncm_sf_spherical_harmonics_get_lmax (spha));
			break;
		case PROP_ABSTOL:
			g_value_set_double (value, ncm_sf_spherical_harmonics_get_abstol (spha));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
_ncm_sf_spherical_harmonics_dispose (GObject *object)
{
	NcmSFSphericalHarmonics *spha = NCM_SF_SPHERICAL_HARMONICS (object);

	g_clear_pointer (&spha->sqrt_n, g_array_unref);
	g_clear_pointer (&spha->sqrtm1_n, g_array_unref);
	g_clear_pointer (&spha->K_array, g_ptr_array_unref);
	
	/* Chain up : end */
	G_OBJECT_CLASS (ncm_sf_spherical_harmonics_parent_class)->dispose (object);
}

static void
_ncm_sf_spherical_harmonics_finalize (GObject *object)
{

	/* Chain up : end */
	G_OBJECT_CLASS (ncm_sf_spherical_harmonics_parent_class)->finalize (object);
}

static void
ncm_sf_spherical_harmonics_class_init (NcmSFSphericalHarmonicsClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = &_ncm_sf_spherical_harmonics_set_property;
	object_class->get_property = &_ncm_sf_spherical_harmonics_get_property;
	object_class->dispose      = &_ncm_sf_spherical_harmonics_dispose;
	object_class->finalize     = &_ncm_sf_spherical_harmonics_finalize;

	g_object_class_install_property (object_class,
	                                 PROP_LMAX,
	                                 g_param_spec_int ("lmax",
	                                                   NULL,
	                                                   "max l",
	                                                   0, G_MAXINT, 1024,
	                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
	g_object_class_install_property (object_class,
	                                 PROP_ABSTOL,
	                                 g_param_spec_double ("abstol",
	                                                      NULL,
	                                                      "abstol",
	                                                      GSL_DBL_MIN, GSL_DBL_MAX, 1.0e-20,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
}

/**
 * ncm_sf_spherical_harmonics_new:
 * 
 * Creates a new #NcmSFSphericalHarmonics object.
 * 
 * Returns: a new #NcmSFSphericalHarmonics.
 */
NcmSFSphericalHarmonics *
ncm_sf_spherical_harmonics_new (const guint lmax)
{
	NcmSFSphericalHarmonics *spha = g_object_new (NCM_TYPE_SF_SPHERICAL_HARMONICS,
	                                              "lmax", lmax,
	                                              NULL);
	return spha;
}


/**
 * ncm_sf_spherical_harmonics_ref:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Increase the reference of @spha by one.
 *
 * Returns: (transfer full): @spha.
 */
NcmSFSphericalHarmonics *
ncm_sf_spherical_harmonics_ref (NcmSFSphericalHarmonics *spha)
{
  return g_object_ref (spha);
}

/**
 * ncm_sf_spherical_harmonics_free:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Decrease the reference count of @spha by one.
 *
 */
void
ncm_sf_spherical_harmonics_free (NcmSFSphericalHarmonics *spha)
{
  g_object_unref (spha);
}

/**
 * ncm_sf_spherical_harmonics_clear:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Decrease the reference count of @spha by one, and sets the pointer *@spha to
 * NULL.
 *
 */
void
ncm_sf_spherical_harmonics_clear (NcmSFSphericalHarmonics **spha)
{
  g_clear_object (spha);
}

#define SN(n)   g_array_index (spha->sqrt_n, gdouble, (n))
#define SNM1(n) g_array_index (spha->sqrtm1_n, gdouble, (n))

/**
 * ncm_sf_spherical_harmonics_set_lmax:
 * @spha: a #NcmSFSphericalHarmonics
 * @lmax: maximum l
 *
 * Sets the maximum l to @lmax.
 * 
 */
void 
ncm_sf_spherical_harmonics_set_lmax (NcmSFSphericalHarmonics *spha, const guint lmax)
{
	if (lmax != spha->lmax)
	{
		const gint nmax_old = spha->sqrt_n->len;
		const gint nmax     = 2 * lmax + 3;
		gint n;

		g_array_set_size (spha->sqrt_n,   nmax + 1);
		g_array_set_size (spha->sqrtm1_n, nmax + 1);

		for (n = nmax_old; n <= nmax; n++)
		{
			const gdouble sqrt_n = sqrt (n);
			g_array_index (spha->sqrt_n, gdouble, n)   = sqrt_n;
			g_array_index (spha->sqrtm1_n, gdouble, n) = 1.0 / sqrt_n;
		}

		for (n = 0; n <= lmax; n++)
		{
			GArray *Km_array = NULL;
			gint l;

			if (n < spha->K_array->len)
			{
				Km_array = g_ptr_array_index (spha->K_array, n);
			}
			else if (n == spha->K_array->len)
			{
				Km_array = g_array_new (TRUE, TRUE, sizeof (NcmSFSphericalHarmonicsK));
				g_ptr_array_add (spha->K_array, Km_array);
			}
			else
			{
				g_assert_not_reached ();
			}

			for (l = Km_array->len + n; l < lmax; l++)
			{
				NcmSFSphericalHarmonicsK Km;
				const gint m       = n;
				const gint twol    = 2 * l;
				const gint lmm     = l - m;
				const gint lpm     = l + m;
				const gdouble pref = SN (twol + 5) * SNM1 (lpm + 2) * SNM1 (lmm + 2);

				Km.lp1  = pref * SN (twol + 3);
				Km.l    = pref * SN (lmm + 1) * SN (lpm + 1) * SNM1 (twol + 1);

				g_array_append_val (Km_array, Km);
			}
		}

		spha->lmax = lmax;
	}
}

/**
 * ncm_sf_spherical_harmonics_get_lmax:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Gets the maximum l to @lmax.
 * 
 * Returns: the currently used lmax.
 */
guint 
ncm_sf_spherical_harmonics_get_lmax (NcmSFSphericalHarmonics *spha)
{
	return spha->lmax;
}

/**
 * ncm_sf_spherical_harmonics_set_abstol:
 * @spha: a #NcmSFSphericalHarmonics
 * @abstol: the absolute tolerance
 *
 * Sets the absolute tolerance to @abstol.
 * 
 */
void 
ncm_sf_spherical_harmonics_set_abstol (NcmSFSphericalHarmonics *spha, const gdouble abstol)
{
	spha->abstol = abstol;
}

/**
 * ncm_sf_spherical_harmonics_get_abstol:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current abstol.
 */
gdouble 
ncm_sf_spherical_harmonics_get_abstol (NcmSFSphericalHarmonics *spha)
{
	return spha->abstol;
}

/**
 * ncm_sf_spherical_harmonics_start_rec:
 * @spha: a #NcmSFSphericalHarmonics
 * @x: $x$
 * @sqrt1mx2: $\sqrt{1-x^2}$
 * 
 * Start recursion for $x$ at $l = 0,\; m = 0$.
 *
 */
/**
 * ncm_sf_spherical_harmonics_next_l:
 * @spha: a #NcmSFSphericalHarmonics
 * 
 * Move the recursion for $x$ to $l = l + 1$.
 *
 */
/**
 * ncm_sf_spherical_harmonics_next_m:
 * @spha: a #NcmSFSphericalHarmonics
 * 
 * Restart the recursion for $x$ at $l = m + 1,\; m = m + 1$.
 *
 */

/**
 * ncm_sf_spherical_harmonics_get_Yblm:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current value of $\bar{Y}_l^m (x)$.
 */
/**
 * ncm_sf_spherical_harmonics_get_Yblp1m:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current value of $\bar{Y}_{l+1}^m (x)$.
 */
/**
 * ncm_sf_spherical_harmonics_get_x:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current value of $x$.
 */
/**
 * ncm_sf_spherical_harmonics_get_l:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current value of $l$.
 */
/**
 * ncm_sf_spherical_harmonics_get_m:
 * @spha: a #NcmSFSphericalHarmonics
 *
 * Returns: the current value of $m$.
 */
