/***************************************************************************
 *            nc_hipert_wkb.h
 *
 *  Sun August 03 20:39:19 2014
 *  Copyright  2014  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * nc_hipert_wkb.h
 * Copyright (C) 2014 Sandro Dias Pinto Vitenti <sandro@isoftware.com.br>
 *
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

#ifndef _NC_HIPERT_WKB_H_
#define _NC_HIPERT_WKB_H_

#include <glib.h>
#include <glib-object.h>
#include <numcosmo/build_cfg.h>
#include <numcosmo/math/ncm_ode_spline.h>
#include <numcosmo/nc_hicosmo.h>
#include <numcosmo/perturbations/nc_hipert.h>

G_BEGIN_DECLS

#define NC_TYPE_HIPERT_WKB             (nc_hipert_wkb_get_type ())
#define NC_HIPERT_WKB(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), NC_TYPE_HIPERT_WKB, NcHIPertWKB))
#define NC_HIPERT_WKB_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), NC_TYPE_HIPERT_WKB, NcHIPertWKBClass))
#define NC_IS_HIPERT_WKB(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NC_TYPE_HIPERT_WKB))
#define NC_IS_HIPERT_WKB_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), NC_TYPE_HIPERT_WKB))
#define NC_HIPERT_WKB_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), NC_TYPE_HIPERT_WKB, NcHIPertWKBClass))

typedef struct _NcHIPertWKBClass NcHIPertWKBClass;
typedef struct _NcHIPertWKB NcHIPertWKB;

struct _NcHIPertWKBClass
{
  /*< private >*/
  NcHIPertClass parent_class;
};

/**
 * NcHIPertWKBVars:
 * @NC_HIPERT_WKB_RE_Q: $\text{Re}(q)$
 * @NC_HIPERT_WKB_IM_Q: $\text{Im}(q)$
 * @NC_HIPERT_WKB_RE_P: $\text{Re}(p)$
 * @NC_HIPERT_WKB_IM_P: $\text{Im}(p)$
 * 
 * Perturbation variables enumerator.
 * 
 */
typedef enum _NcHIPertWKBVars
{
  NC_HIPERT_WKB_RE_Q = 0,
  NC_HIPERT_WKB_IM_Q,
  NC_HIPERT_WKB_RE_P,
  NC_HIPERT_WKB_IM_P,
} NcHIPertWKBVars;

typedef gdouble (*NcHIPertWKBFunc) (GObject *obj, gdouble alpha, gdouble k);
typedef void (*NcHIPertWKBEom) (GObject *obj, gdouble alpha, gdouble k, gdouble *nu2, gdouble *m, gdouble *dlnm);

struct _NcHIPertWKB
{
  /*< private >*/
  NcHIPert parent_instance;
  GType impl_type;
  GObject *obj;
  NcmOdeSpline *phase;
  NcmSpline *lnphase;
  NcmSpline *lnF;
  NcmSpline *dlnF;
  gboolean prep;
  gboolean exact_prep;
  gboolean patched_prep;
  gdouble alpha_i;
  gdouble alpha_f;
  gdouble alpha_exact_i;
  gdouble alpha_exact_f;
  gdouble alpha_patched_i;
  gdouble alpha_patched_f;
  NcHIPertWKBFunc V;
  NcHIPertWKBFunc nuA2;
  NcHIPertWKBFunc dmnuA_nuA;
  NcHIPertWKBEom eom;
};

GType nc_hipert_wkb_get_type (void) G_GNUC_CONST;

NcHIPertWKB *nc_hipert_wkb_new (GType impl_type, NcHIPertWKBFunc nuA2, NcHIPertWKBFunc V, NcHIPertWKBFunc dmnuA_nuA, NcHIPertWKBEom eom);
NcHIPertWKB *nc_hipert_wkb_ref (NcHIPertWKB *wkb);
void nc_hipert_wkb_free (NcHIPertWKB *wkb);
void nc_hipert_wkb_clear (NcHIPertWKB **wkb);

void nc_hipert_wkb_prepare (NcHIPertWKB *wkb, GObject *obj, gdouble alpha_i, gdouble alpha_f);
void nc_hipert_wkb_prepare_exact (NcHIPertWKB *wkb, GObject *obj, gdouble alpha_i, gdouble alpha_f);
void nc_hipert_wkb_prepare_patched (NcHIPertWKB *wkb, GObject *obj, gdouble prec, gdouble alpha_i, gdouble alpha_f);

void nc_hipert_wkb_q (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q);
void nc_hipert_wkb_q_p (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q, gdouble *Re_p, gdouble *Im_p);

void nc_hipert_wkb_exact_q (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q);
void nc_hipert_wkb_exact_q_p (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q, gdouble *Re_p, gdouble *Im_p);

void nc_hipert_wkb_patched_q (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q);
void nc_hipert_wkb_patched_q_p (NcHIPertWKB *wkb, GObject *obj, gdouble alpha, gdouble *Re_q, gdouble *Im_q, gdouble *Re_p, gdouble *Im_p);

gdouble nc_hipert_wkb_nuA (NcHIPertWKB *wkb, GObject *obj, gdouble alpha);

gdouble nc_hipert_wkb_maxtime (NcHIPertWKB *wkb, GObject *obj, gdouble alpha0, gdouble alpha1);
gdouble nc_hipert_wkb_maxtime_prec (NcHIPertWKB *wkb, GObject *obj, gdouble prec, gdouble alpha0, gdouble alpha1);

G_END_DECLS

#endif /* _NC_HIPERT_WKB_H_ */

