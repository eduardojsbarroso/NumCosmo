/***************************************************************************
 *            ncm_mset.c
 *
 *  Fri May 25 09:37:54 2012
 *  Copyright  2012  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * numcosmo
 * Copyright (C) Sandro Dias Pinto Vitenti 2012 <sandro@isoftware.com.br>
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

/**
 * SECTION:ncm_mset
 * @title: NcmMSet
 * @short_description: A set of different NcmModel objects.
 *
 * FIXME
 * 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */
#include "build_cfg.h"

#include "math/ncm_mset.h"
#include "math/ncm_serialize.h"
#include "math/ncm_cfg.h"

enum
{
  PROP_0,
  PROP_VALID_MAP,
  PROP_SIZE,
};

G_DEFINE_TYPE (NcmMSet, ncm_mset, G_TYPE_OBJECT);
G_DEFINE_BOXED_TYPE (NcmMSetPIndex, ncm_mset_pindex, ncm_mset_pindex_dup, ncm_mset_pindex_free);

/**
 * ncm_mset_pindex_new:
 * @mid: Model id
 * @pid: Parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSetPIndex *
ncm_mset_pindex_new (NcmModelID mid, guint pid)
{
  NcmMSetPIndex *pi = g_slice_new0 (NcmMSetPIndex);
  pi->mid = mid;
  pi->pid = pid;
    
  return pi;
}

/**
 * ncm_mset_pindex_dup:
 * @pi: a #NcmMSetPIndex
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSetPIndex *
ncm_mset_pindex_dup (NcmMSetPIndex *pi)
{
  NcmMSetPIndex *new_pi = ncm_mset_pindex_new (pi->mid, pi->pid);
  return new_pi;
}

/**
 * ncm_mset_pindex_free:
 * @pi: a #NcmMSetPIndex
 *
 * FIXME
 *
 */
void
ncm_mset_pindex_free (NcmMSetPIndex *pi)
{
  g_slice_free (NcmMSetPIndex, pi);
}

G_LOCK_DEFINE_STATIC (last_model_id);

/**
 * ncm_mset_model_register_id: (skip)
 * @model_class: a #NcmModelClass
 * @ns: model namespace
 * @desc: short description
 * @long_desc: long description
 * 
 * FIXME
 * 
 */ 
void
ncm_mset_model_register_id (NcmModelClass *model_class, const gchar *ns, const gchar *desc, const gchar *long_desc)
{
  if (model_class->model_id < 0)
  {
    static NcmModelID last_model_id = 0;
    guint id;
    NcmMSetClass *mset_class = g_type_class_ref (NCM_TYPE_MSET);
    G_LOCK (last_model_id);
    model_class->model_id = last_model_id++;
    id = model_class->model_id;
    mset_class->model_desc[id].init = TRUE;
    if (ns == NULL)
      g_error ("Cannot register model without a namespace.");
    if (desc == NULL)
      g_error ("Cannot register model without a description.");

    mset_class->model_desc[id].ns = g_strdup (ns);
    mset_class->model_desc[id].desc = g_strdup (desc);
    if (long_desc != NULL)
      mset_class->model_desc[id].long_desc = g_strdup (long_desc);
    else
      mset_class->model_desc[id].long_desc = NULL;

    if (g_hash_table_lookup (mset_class->ns_table, ns) != NULL)
      g_error ("Model namespace <%s> already registered.", ns);

    g_hash_table_insert (mset_class->ns_table, 
                         mset_class->model_desc[id].ns, 
                         GINT_TO_POINTER (id));

    G_UNLOCK (last_model_id);
    g_type_class_unref (mset_class);
  }
  else
  {
    g_error ("This model or its parent is already registred, id = %d. This function must be use once and only in the defining model.", model_class->model_id);
  }
  if (model_class->model_id > NCM_MODEL_MAX_ID)
    g_error ("Max model id was already attained. Increase by altering NCM_MODEL_MAX_ID.");

  return;
}

/**
 * ncm_mset_empty_new:
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSet *
ncm_mset_empty_new (void)
{
  return g_object_new (NCM_TYPE_MSET, NULL);
}

/**
 * ncm_mset_new:
 * @model0: a #NcmModel
 * @...: FIXME
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSet *
ncm_mset_new (NcmModel *model0, ...)
{
  NcmMSet *mset;
  va_list ap;

  va_start (ap, model0);
  mset = ncm_mset_newv (model0, ap);
  va_end (ap);

  return mset;
}

/**
 * ncm_mset_newv:
 * @model0: a #NcmModel
 * @ap: FIXME
 *
 * FIXME
 *
 * Returns: (transfer full): FIXME
 */
NcmMSet *
ncm_mset_newv (NcmModel *model0, va_list ap)
{
  NcmMSet *mset = ncm_mset_empty_new ();
  NcmModel *model = NULL;

  ncm_mset_set (mset, model0);

  while ((model = va_arg (ap, NcmModel *)) != NULL)
    ncm_mset_set (mset, model);

  return mset;
}

/**
 * ncm_mset_ref:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: (transfer full): a new #NcmMSet
 */
NcmMSet *
ncm_mset_ref (NcmMSet *mset)
{
  return g_object_ref (mset);
}

/**
 * ncm_mset_dup:
 * @mset: a #NcmMSet
 * @ser: a #NcmSerialize
 *
 * FIXME
 *
 * Returns: (transfer full): a new #NcmMSet
 */
NcmMSet *
ncm_mset_dup (NcmMSet *mset, NcmSerialize *ser)
{
  return NCM_MSET (ncm_serialize_dup_obj (ser, G_OBJECT (mset)));
}

/**
 * ncm_mset_free:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 */
void
ncm_mset_free (NcmMSet *mset)
{
  g_object_unref (mset);
}

/**
 * ncm_mset_clear:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 */
void
ncm_mset_clear (NcmMSet **mset)
{
  g_clear_object (mset);
}

/**
 * ncm_mset_peek:
 * @mset: a #NcmMSet
 * @mid: a #NcmModelID
 *
 * FIXME
 *
 * Returns: (transfer none): FIXME
 */
NcmModel *
ncm_mset_peek (NcmMSet *mset, NcmModelID mid)
{
  return mset->model[mid];
}

/**
 * ncm_mset_get:
 * @mset: a #NcmMSet
 * @mid: a #NcmModelID
 *
 * FIXME
 *
 * Returns: (transfer full): FIXME
 */
NcmModel *
ncm_mset_get (NcmMSet *mset, NcmModelID mid)
{
  g_assert (mset->model[mid]);
  return ncm_model_ref (mset->model[mid]);
}

/**
 * ncm_mset_remove:
 * @mset: a #NcmMSet
 * @mid: a #NcmModelID
 *
 * FIXME
 *
 */
void
ncm_mset_remove (NcmMSet *mset, NcmModelID mid)
{
  if (mset->model[mid] != NULL)
  {
    mset->total_len -= ncm_model_len (mset->model[mid]);
    ncm_model_free (mset->model[mid]);
    mset->model[mid] = NULL;
    mset->valid_map = FALSE;
  }
}

/**
 * ncm_mset_set:
 * @mset: a #NcmMSet
 * @model: a #NcmModel
 *
 * FIXME
 *
 */
void
ncm_mset_set (NcmMSet *mset, NcmModel *model)
{
  g_assert (model != NULL);
  {
    NcmModelID mid = ncm_model_id (model);
    guint model_len = ncm_model_len (model);
    guint i;

    if (mset->model[mid] != NULL)
      ncm_mset_remove (mset, mid);

    mset->model[mid] = ncm_model_ref (model);

    g_array_set_size (mset->fpi_array[mid], model_len);
    for (i = 0; i < model_len; i++)
      g_array_index (mset->fpi_array[mid], gint, i) = -1;

    mset->total_len += model_len;

    if (mset->valid_map)
      ncm_mset_prepare_fparam_map (mset);
  }
}

/**
 * ncm_mset_exists:
 * @mset: a #NcmMSet
 * @model: a #NcmModel
 *
 * FIXME
 * 
 * Returns: FIXME
 */
gboolean
ncm_mset_exists (NcmMSet *mset, NcmModel *model)
{
  NcmModelID mid = ncm_model_id (model);

  if (mset->model[mid] != NULL)
    return TRUE;
  else
    return FALSE;
}

/**
 * ncm_mset_get_id_by_ns:
 * @ns: FIXME
 *
 * FIXME
 * 
 * Returns: FIXME
 */
gint 
ncm_mset_get_id_by_ns (const gchar *ns)
{
  NcmMSetClass *mset_class = g_type_class_ref (NCM_TYPE_MSET);
  gpointer model_id;
  gboolean has = g_hash_table_lookup_extended (mset_class->ns_table, ns, NULL, &model_id);

  g_type_class_unref (mset_class);
  if (has)
    return GPOINTER_TO_INT (model_id);
  else
    return -1;
}

/**
 * ncm_mset_get_ns_by_id:
 * @id: namespace id
 * 
 * Returns: namespace for @id 
 */
const gchar *
ncm_mset_get_ns_by_id (gint id)
{
  NcmMSetClass *mset_class = g_type_class_ref (NCM_TYPE_MSET);
  g_assert_cmpint (id, <, NCM_MODEL_MAX_ID);
  {
    const gchar *ns = mset_class->model_desc[id].ns;
    g_type_class_unref (mset_class);
    return ns;
  }
}

/**
 * ncm_mset_prepare_fparam_map:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 */
void
ncm_mset_prepare_fparam_map (NcmMSet *mset)
{
  NcmModelID mid;
  guint pid;

  mset->fparam_len = 0;
  g_array_set_size (mset->pi_array, 0);
  g_ptr_array_set_size (mset->fullname_parray, 0);

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      guint n_params = ncm_model_len (model);
      for (pid = 0; pid < n_params; pid++)
      {
        if (ncm_model_param_get_ftype (model, pid) == NCM_PARAM_TYPE_FREE)
        {
          NcmMSetPIndex pi = {mid, pid};
          g_array_append_val (mset->pi_array, pi);
          g_array_index (mset->fpi_array[mid], gint, pid) = mset->fparam_len;
          mset->fparam_len++;
        }
        else
          g_array_index (mset->fpi_array[mid], gint, pid) = -1;
      }
    }
  }

  g_ptr_array_set_size (mset->fullname_parray, mset->fparam_len);
  mset->valid_map = TRUE;
}

/**
 * ncm_mset_total_len:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_total_len (NcmMSet *mset)
{
  return mset->total_len;
}

/**
 * ncm_mset_fparam_len:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_fparam_len (NcmMSet *mset)
{
  return mset->fparam_len;
}

/**
 * ncm_mset_max_param_name:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_max_param_name (NcmMSet *mset)
{
  NcmModelID mid;
  gint i;
  guint name_size = 0;
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      for (i = 0; i < n_params; i++)
      {
        const gchar *pname = ncm_model_param_name (model, i);
        name_size = GSL_MAX (name_size, strlen (pname));
      }
    }
  }
  return name_size;
}

/**
 * ncm_mset_param_len:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint 
ncm_mset_param_len (NcmMSet *mset)
{
  NcmModelID mid;
  guint len = 0;
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
      len += ncm_model_len (model);
  }
  return len;
}

/**
 * ncm_mset_max_fparam_name:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_max_fparam_name (NcmMSet *mset)
{
  guint name_size = 0, i;
  guint free_params_len = ncm_mset_fparams_len (mset);
  for (i = 0; i < free_params_len; i++)
  {
    const gchar *pname = ncm_mset_fparam_name (mset, i);
    name_size = GSL_MAX (name_size, strlen (pname));
  }
  return name_size;
}

/**
 * ncm_mset_max_model_nick:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_max_model_nick (NcmMSet *mset)
{
  NcmModelID mid;
  guint nick_size = 0;
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      const gchar *nick = ncm_model_nick (model);
      nick_size = GSL_MAX (nick_size, strlen (nick));
    }
  }
  return nick_size;
}

/**
 * ncm_mset_pretty_log:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 */
void
ncm_mset_pretty_log (NcmMSet *mset)
{
  NcmMSetClass *mset_class = NCM_MSET_GET_CLASS (mset);
  NcmModelID mid;
  gint i;
  guint name_size = ncm_mset_max_param_name (mset);
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      ncm_cfg_msg_sepa ();
      g_message ("# Model[%02d]:\n#   - %s : %s\n", mid, mset_class->model_desc[mid].ns, ncm_model_name (model));
      ncm_cfg_msg_sepa ();
      g_message ("# Model parameters\n");
      for (i = 0; i < n_params; i++)
      {
        const gchar *pname = ncm_model_param_name (model, i);
        g_message ("#   - %*s[%02d]: % -20.15g [%s]\n", name_size, pname, i,
                   ncm_model_param_get (model, i),
                   (ncm_model_param_get_ftype (model, i) == NCM_PARAM_TYPE_FREE) ? "FREE" : "FIXED");
      }
    }
  }
}

/**
 * ncm_mset_params_pretty_print:
 * @mset: a #NcmMSet
 * @out: name of the file
 * @header: pointer to the command line
 *
 * This function print the command line (first line, commented), the model nick and parameters' names (second line, commented)
 * and their values indicating if they are fixed or free.
 *
 */
void
ncm_mset_params_pretty_print (NcmMSet *mset, FILE *out, const gchar *header)
{
  NcmModelID mid;
  gint i;
  guint name_size = ncm_mset_max_param_name (mset);
  guint nick_size = ncm_mset_max_model_nick (mset);
  name_size = GSL_MAX (name_size, 5);

  if (header != NULL)
    fprintf (out, "# %s\n ", header);
  else
    fprintf (out, "#\n");

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      const gchar *nick = ncm_model_nick (model);
      gint n_params = ncm_model_len (model);
      for (i = 0; i < n_params; i++)
      {
        const gchar *pname = ncm_model_param_name (model, i);
        fprintf (out, "%*s:%*s %*s % 5.5g\n", nick_size, nick, name_size, pname, name_size,
                 (ncm_model_param_get_ftype (model, i) == NCM_PARAM_TYPE_FREE) ? "FREE" : "FIXED",
                 ncm_model_param_get (model, i));
      }
    }
  }

  return;
}

/**
 * ncm_mset_params_log_vals:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 */
void
ncm_mset_params_log_vals (NcmMSet *mset)
{
  NcmModelID mid;
  gint i;
  g_message ("#");
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      for (i = 0; i < n_params; i++)
      {
        g_message (" % -20.15g", ncm_model_param_get (model, i));
      }
    }
  }
  g_message ("\n");
}

/**
 * ncm_mset_params_print_vals:
 * @mset: a #NcmMSet
 * @out: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_params_print_vals (NcmMSet *mset, FILE *out)
{
  NcmModelID mid;
  gint i;
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      for (i = 0; i < n_params; i++)
      {
        fprintf (out, " % -20.15g", ncm_model_param_get (model, i));
      }
    }
  }
  fprintf (out, "\n");
}

/**
 * ncm_mset_fparams_log_covar:
 * @mset: a #NcmMSet
 * @covar: a #NcmMatrix
 *
 * FIXME
 *
 */
void 
ncm_mset_fparams_log_covar (NcmMSet *mset, NcmMatrix *covar)
{
  guint i, j;
  guint name_size = ncm_mset_max_fparam_name (mset);
  guint free_params_len = ncm_mset_fparam_len (mset);
  const gchar *box = "---------------";
  g_assert (covar != NULL);

  ncm_cfg_msg_sepa ();
  g_message ("# NcmMSet parameters covariance matrix\n");
  g_message ("#                                        ");
  for (i = 0; i < name_size; i++) g_message (" ");

  for (i = 0; i < free_params_len; i++)
    i ? g_message ("%s",box) : g_message ("-%s",box);
  if (i)
    g_message ("\n");

  for (i = 0; i < free_params_len; i++)
  {
    NcmMSetPIndex *pi = ncm_mset_fparam_get_pi (mset, i);
    const gchar *pname = ncm_mset_fparam_name (mset, i);
    g_message ("# %*s[%02d%02d] = % -12.4g +/- % -12.4g |",
               name_size, pname, pi->mid, pi->pid,
               ncm_mset_fparam_get (mset, i),
               sqrt (ncm_matrix_get (covar, i, i))
               );
    for (j = 0; j < free_params_len; j++)
    {
      g_message (" % -12.4g |", ncm_matrix_get (covar, i, j) / (sqrt (ncm_matrix_get (covar, i, i) * ncm_matrix_get (covar, j, j))));
    }
    g_message ("\n");
  }
  g_message ("#                                        ");
  for (i = 0; i < name_size; i++) g_message (" ");
  for (i = 0; i < free_params_len; i++)
    i ? g_message ("%s",box) : g_message ("-%s",box);
  if (i)
    g_message ("\n");

  return;
}

/**
 * ncm_mset_params_valid:
 * @mset: a #NcmMSet
 *
 * FIXME
 * 
 * Returns: FIXME
 */
gboolean 
ncm_mset_params_valid (NcmMSet *mset)
{
  NcmModelID mid;

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else if (!ncm_model_params_valid (model))
      return FALSE;
  }
  return TRUE;
}

/**
 * ncm_mset_params_bounds:
 * @mset: a #NcmMSet
 *
 * Check whenever the parameters respect the bounds.
 * 
 * Returns: If TRUE the parameter respect the bounds.
 */
gboolean 
ncm_mset_params_valid_bounds (NcmMSet *mset)
{
  NcmModelID mid;

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else if (!ncm_model_params_valid_bounds (model))
      return FALSE;
  }
  return TRUE;
}

/**
 * ncm_mset_cmp:
 * @mset0: a #NcmMSet
 * @mset1: a #NcmMSet
 * @cmp_model: FIXME
 *
 * Compares @mset0 and @mset1 and returns TRUE if both coitains the same models.
 * If @cmp_model is TRUE compare also if the models are the same.
 * 
 * Returns: FIXME
 */
gboolean 
ncm_mset_cmp (NcmMSet *mset0, NcmMSet *mset1, gboolean cmp_model)
{
  NcmModelID mid;
  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model0 = ncm_mset_peek (mset0, mid);
    NcmModel *model1 = ncm_mset_peek (mset1, mid);

    if (model0 == NULL && model1 == NULL)
      continue;
    else if (model0 == NULL && model1 != NULL)
      return FALSE;
    else if (model1 == NULL && model0 != NULL)
      return FALSE;
    else if (cmp_model && !ncm_model_is_equal (model0, model1))
      return FALSE;
  }
  return TRUE;
}

/**
 * ncm_mset_param_set:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 * @x: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_param_set (NcmMSet *mset, NcmModelID mid, guint pid, const gdouble x)
{
  ncm_model_param_set (ncm_mset_peek (mset, mid), pid, x);
}

/**
 * ncm_mset_param_get:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_param_get (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_get (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_orig_param_get:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_orig_param_get (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_orig_param_get (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_name:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
const gchar *
ncm_mset_param_name (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_name (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_symbol:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
const gchar *
ncm_mset_param_symbol (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_symbol (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_get_scale:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_param_get_scale (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_get_scale (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_set_scale:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 * @scale: new scale
 *
 * FIXME
 *
 */
void
ncm_mset_param_set_scale (NcmMSet *mset, NcmModelID mid, guint pid, gdouble scale)
{
  ncm_model_param_set_scale (ncm_mset_peek (mset, mid), pid, scale);
}

/**
 * ncm_mset_param_get_lower_bound:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_param_get_lower_bound (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_get_lower_bound (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_get_upper_bound:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_param_get_upper_bound (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_get_upper_bound (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_get_abstol:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_param_get_abstol (NcmMSet *mset, NcmModelID mid, guint pid)
{
  return ncm_model_param_get_abstol (ncm_mset_peek (mset, mid), pid);
}

/**
 * ncm_mset_param_set_ftype:
 * @mset: a #NcmMSet
 * @mid: model id
 * @pid: parameter id
 * @ftype: a #NcmParamType
 *
 * FIXME
 *
 */
void
ncm_mset_param_set_ftype (NcmMSet *mset, NcmModelID mid, guint pid, NcmParamType ftype)
{
  NcmModel *model = ncm_mset_peek (mset, mid);
  g_assert (model != NULL);
  ncm_model_param_set_ftype (model, pid, ftype);
}

/**
 * ncm_mset_param_set_all_ftype:
 * @mset: a #NcmMSet
 * @ftype: a #NcmParamType
 *
 * FIXME
 *
 */
void
ncm_mset_param_set_all_ftype (NcmMSet *mset, NcmParamType ftype)
{
  NcmModelID mid;
  gint pid;

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      for (pid = 0; pid < n_params; pid++)
      {
        ncm_model_param_set_ftype (ncm_mset_peek (mset, mid), pid, ftype);
      }
    }
  }
}

/**
 * ncm_mset_param_set_vector:
 * @mset: a #NcmMSet
 * @params: a #NcmVector
 *
 * Sets the models parameters using values from @params.
 *
 */
void 
ncm_mset_param_set_vector (NcmMSet *mset, NcmVector *params)
{
  NcmModelID mid;
  guint j = 0;

  g_assert (ncm_vector_len (params) == ncm_mset_param_len (mset));

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      NcmVector *mvec = ncm_vector_get_subvector (params, j, n_params);
      ncm_model_params_set_vector (model, mvec);
      ncm_vector_free (mvec);
      j += n_params;
    }
  }
}

/**
 * ncm_mset_param_get_vector:
 * @mset: a #NcmMSet
 * @params: a #NcmVector
 *
 * Sets the compontents of @params using the models parameters.
 *
 */
void 
ncm_mset_param_get_vector (NcmMSet *mset, NcmVector *params)
{
  NcmModelID mid;
  gint pid;
  guint j = 0;

  g_assert (ncm_vector_len (params) == ncm_mset_param_len (mset));

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    NcmModel *model = ncm_mset_peek (mset, mid);
    if (model == NULL)
      continue;
    else
    {
      gint n_params = ncm_model_len (model);
      for (pid = 0; pid < n_params; pid++)
      {
        ncm_vector_set (params, j++, ncm_model_param_get (model, pid));
      }
    }
  }
}

/**
 * ncm_mset_param_get_ftype:
 * @mset: a #NcmMSet
 * @mid: a #NcmModelID
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmParamType
ncm_mset_param_get_ftype (NcmMSet *mset, NcmModelID mid, guint pid)
{
  NcmModel *model = ncm_mset_peek (mset, mid);
  if (model == NULL)
    g_error ("ncm_mset_param_get_ftype: cannot get ftype of mode %d, model not set.", mid);
  return ncm_model_param_get_ftype (model, pid);
}

/**
 * ncm_mset_param_set_pi:
 * @mset: a #NcmMSet
 * @pi: a #NcmMSetPIndex
 * @x: FIXME
 * @n: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_param_set_pi (NcmMSet *mset, NcmMSetPIndex *pi, const gdouble *x, guint n)
{
  guint fpi;
  for (fpi = 0; fpi < n; fpi++)
    ncm_mset_param_set (mset, pi[fpi].mid, pi[fpi].pid, x[fpi]);
}

/**
 * ncm_mset_param_get_pi:
 * @mset: a #NcmMSet
 * @pi: a #NcmMSetPIndex
 * @x: FIXME
 * @n: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_param_get_pi (NcmMSet *mset, NcmMSetPIndex *pi, gdouble *x, guint n)
{
  guint fpi;
  for (fpi = 0; fpi < n; fpi++)
    x[fpi] = ncm_mset_param_get (mset, pi[fpi].mid, pi[fpi].pid);
}

/**
 * ncm_mset_fparams_get_vector:
 * @mset: a #NcmMSet
 * @x: a #NcmVector
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_get_vector (NcmMSet *mset, NcmVector *x)
{
  guint fpi;

  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_vector_set (x, fpi, ncm_mset_param_get (mset, pi.mid, pi.pid));
  }
}

/**
 * ncm_mset_fparams_get_vector_offset:
 * @mset: a #NcmMSet
 * @x: a #NcmVector
 * @offset: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_get_vector_offset (NcmMSet *mset, NcmVector *x, guint offset)
{
  guint fpi;

  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_vector_set (x, fpi + offset, ncm_mset_param_get (mset, pi.mid, pi.pid));
  }
}

/**
 * ncm_mset_fparams_set_vector:
 * @mset: a #NcmMSet
 * @x: a #NcmVector
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_set_vector (NcmMSet *mset, const NcmVector *x)
{
  guint fpi;

  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_mset_param_set (mset, pi.mid, pi.pid, ncm_vector_get (x, fpi));
  }
}

/**
 * ncm_mset_fparams_set_vector_offset:
 * @mset: a #NcmMSet
 * @x: a #NcmVector
 * @offset: starting index
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_set_vector_offset (NcmMSet *mset, const NcmVector *x, guint offset)
{
  guint fpi;

  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_mset_param_set (mset, pi.mid, pi.pid, ncm_vector_get (x, fpi + offset));
  }
}

/**
 * ncm_mset_fparams_set_array:
 * @mset: a #NcmMSet
 * @x: (array) (element-type double): FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_set_array (NcmMSet *mset, const gdouble *x)
{
  guint fpi;
  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_mset_param_set (mset, pi.mid, pi.pid, x[fpi]);
  }
}

/**
 * ncm_mset_fparams_set_gsl_vector: (skip)
 * @mset: a #NcmMSet.
 * @x: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_fparams_set_gsl_vector (NcmMSet *mset, const gsl_vector *x)
{
  guint fpi;
  for (fpi = 0; fpi < mset->fparam_len; fpi++)
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, fpi);
    ncm_mset_param_set (mset, pi.mid, pi.pid, gsl_vector_get (x, fpi));
  }
}

/**
 * ncm_mset_fparams_len:
 * @mset: a #NcmMSet
 *
 * FIXME
 *
 * Returns: FIXME
 */
guint
ncm_mset_fparams_len (NcmMSet *mset)
{
  g_assert (mset->valid_map);
  return mset->fparam_len;
}

/**
 * ncm_mset_fparam_name:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: (transfer none): FIXME
 */
const gchar *
ncm_mset_fparam_name (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_name (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_symbol:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: (transfer none): FIXME
 */
const gchar *
ncm_mset_fparam_symbol (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_symbol (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_full_name:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: (transfer none): FIXME
 */
const gchar *
ncm_mset_fparam_full_name (NcmMSet *mset, guint n)
{
  gchar *fullname;
  g_assert (mset->valid_map && n < mset->fparam_len);
  if ((fullname = g_ptr_array_index (mset->fullname_parray, n)) != NULL)
    return fullname;
  else
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    const gchar *model_ns = ncm_mset_get_ns_by_id (pi.mid); /* ncm_model_nick (ncm_mset_peek (mset, pi.mid));*/
    const gchar *pname = ncm_mset_param_name (mset, pi.mid, pi.pid);
    fullname = g_strdup_printf ("%s:%s", model_ns, pname);
    g_ptr_array_index (mset->fullname_parray, n) = fullname;
  }
  return fullname;
}

/**
 * ncm_mset_fparam_get_scale:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_fparam_get_scale (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_get_scale (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_get_lower_bound:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_fparam_get_lower_bound (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_get_lower_bound (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_get_upper_bound:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_fparam_get_upper_bound (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_get_upper_bound (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_get_abstol:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_fparam_get_abstol (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_get_abstol (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_set_scale:
 * @mset: a #NcmMSet
 * @n: free parameter index
 * @scale: new scale
 *
 * FIXME
 * 
 */
void
ncm_mset_fparam_set_scale (NcmMSet *mset, guint n, gdouble scale)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    ncm_mset_param_set_scale (mset, pi.mid, pi.pid, scale);
  }
}

/**
 * ncm_mset_fparam_valid_bounds:
 * @mset: a #NcmMSet
 * @theta: free parameters vector
 *
 * FIXME
 * 
 * Returns: whether @theta contain values respecting the parameter bounds.
 */
gboolean
ncm_mset_fparam_valid_bounds (NcmMSet *mset, NcmVector *theta)
{
  g_assert (mset->valid_map && ncm_vector_len (theta) == mset->fparam_len);
  {
    guint i;
    for (i = 0; i < mset->fparam_len; i++)
    {
      const gdouble lb  = ncm_mset_fparam_get_lower_bound (mset, i);
      const gdouble ub  = ncm_mset_fparam_get_upper_bound (mset, i);
      const gdouble val = ncm_vector_get (theta, i);
      if (val < lb || val > ub)
        return FALSE;
    }
    return TRUE;
  }
}

/**
 * ncm_mset_fparam_get:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
gdouble
ncm_mset_fparam_get (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_get (mset, pi.mid, pi.pid);
  }
}

/**
 * ncm_mset_fparam_set:
 * @mset: a #NcmMSet
 * @n: free parameter index
 * @x: FIXME
 *
 * FIXME
 *
 */
void
ncm_mset_fparam_set (NcmMSet *mset, guint n, const gdouble x)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  {
    const NcmMSetPIndex pi = g_array_index (mset->pi_array, NcmMSetPIndex, n);
    return ncm_mset_param_set (mset, pi.mid, pi.pid, x);
  }
}

/**
 * ncm_mset_fparam_get_pi:
 * @mset: a #NcmMSet
 * @n: free parameter index
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSetPIndex *
ncm_mset_fparam_get_pi (NcmMSet *mset, guint n)
{
  g_assert (mset->valid_map && n < mset->fparam_len);
  return &(g_array_index (mset->pi_array, NcmMSetPIndex, n));
}

/**
 * ncm_mset_fparam_get_fpi:
 * @mset: a #NcmMSet
 * @mid: a #NcmModelID
 * @pid: parameter id
 *
 * FIXME
 *
 * Returns: FIXME
 */
gint
ncm_mset_fparam_get_fpi (NcmMSet *mset, NcmModelID mid, guint pid)
{
  g_assert (mset->valid_map);
  return g_array_index (mset->fpi_array[mid], gint, pid);
}

/**
 * ncm_mset_fparam_get_pi_by_name:
 * @mset: a #NcmMSet
 * @name: FIXME
 *
 * FIXME
 *
 * Returns: FIXME
 */
NcmMSetPIndex *
ncm_mset_fparam_get_pi_by_name (NcmMSet *mset, const gchar *name)
{
  guint match = 0;
  guint match_i = 0;
  guint i;
  g_assert (mset->valid_map);
  for (i = 0; i < mset->fparam_len; i++)
  {
    const gchar *name_i = ncm_mset_fparam_name (mset, i);
    if (strcmp (name, name_i) == 0)
    {
      match++;
      match_i = i;
    }
  }

  if (match > 1)
  {
    g_warning ("ncm_mset_fparam_get_pi_by_name: more than one [%u] parameters with the same name %s, use the full name to avoid ambiguities, returning the last match.",
               match, name);
    return NULL;
  }
  else if (match == 1)
  {
    return ncm_mset_fparam_get_pi (mset, match_i);
  }
  else
  {
    for (i = 0; i < mset->fparam_len; i++)
    {
      const gchar *name_i = ncm_mset_fparam_full_name (mset, i);
      if (strcmp (name, name_i) == 0)
      {
        match++;
        match_i = i;
      }
    }
    if (match == 0)
      return NULL;
    else if (match > 1)
    {
      g_error ("ncm_mset_fparam_get_pi_by_name: more than one full names [%u] match %s.", match, name);
      return NULL;
    }
    else
      return ncm_mset_fparam_get_pi (mset, match_i);
  }
}

/**
 * ncm_mset_save:
 * @mset: a #NcmMSet
 * @ser: a #NcmSerialize
 * @filename: FIXME
 * @save_comment: FIXME
 * 
 * FIXME
 * 
 */
void 
ncm_mset_save (NcmMSet *mset, NcmSerialize *ser, const gchar *filename, gboolean save_comment)
{
  NcmMSetClass *mset_class = NCM_MSET_GET_CLASS (mset);
  GKeyFile *msetfile = g_key_file_new ();
  guint i;

  {
    GError *error = NULL;
    gchar *mset_desc = ncm_cfg_string_to_comment ("NcmMSet");
    gchar *mset_valid_map_desc = ncm_cfg_string_to_comment ("valid-map property");
    
    g_key_file_set_boolean (msetfile, "NcmMSet", "valid_map", mset->valid_map);
    
    if (!g_key_file_set_comment (msetfile, "NcmMSet", NULL, mset_desc, &error))
      g_error ("ncm_mset_save: %s", error->message);
    if (!g_key_file_set_comment (msetfile, "NcmMSet", "valid_map", mset_valid_map_desc, &error))
      g_error ("ncm_mset_save: %s", error->message);

    g_free (mset_desc);
    g_free (mset_valid_map_desc);
  }
  
  for (i = 0; i < NCM_MODEL_MAX_ID; i++)
  {
    NcmModel *model = ncm_mset_peek (mset, i);
    if (model == NULL)
      continue;
    else
    {
      NcmModelClass *model_class = NCM_MODEL_GET_CLASS (model);
      GObjectClass *oclass = G_OBJECT_CLASS (model_class);
      GError *error = NULL;
      GVariant *params = NULL;
      gchar *obj_name = NULL;
      gchar *group = g_strdup_printf ("%s", mset_class->model_desc[i].ns);
      GVariant *model_var = ncm_serialize_to_variant (ser, G_OBJECT (model));
      guint nparams;

      g_variant_get (model_var, "{s@a{sv}}", &obj_name, &params);
      nparams = g_variant_n_children (params);
      g_key_file_set_value (msetfile, group, group, obj_name);

      if (save_comment)
      {
        gchar *model_desc = ncm_cfg_string_to_comment (mset_class->model_desc[i].desc); 
        if (!g_key_file_set_comment (msetfile, group, NULL, model_desc, &error))
          g_error ("ncm_mset_save: %s", error->message);
        g_free (model_desc);
      }
      
      if (nparams != 0)
      {
        GVariantIter iter;
        GVariant *value;
        gchar *key;
        g_variant_iter_init (&iter, params);
        while (g_variant_iter_next (&iter, "{sv}", &key, &value))
        {
          GParamSpec *param_spec = g_object_class_find_property (oclass, key);
          gchar *param_str = g_variant_print (value, TRUE);
          if (param_spec == NULL)
            g_error ("ncm_mset_save: property `%s' not found in object `%s'.", key, obj_name);
          
          g_key_file_set_value (msetfile, group, key, param_str);
          if (save_comment)
          {
            gchar *desc = ncm_cfg_string_to_comment (g_param_spec_get_blurb (param_spec));
            if (!g_key_file_set_comment (msetfile, group, key, desc, &error))
              g_error ("ncm_mset_save: %s", error->message);
            g_free (desc);
          }

          g_variant_unref (value);
          g_free (key);
          g_free (param_str);
        }
      }

      g_variant_unref (params);
      g_variant_unref (model_var);
    }
  }

  {
    GError *error = NULL;
    gsize len = 0;
    gchar *mset_data = g_key_file_to_data (msetfile, &len, &error);
    if (error != NULL)
      g_error ("Error converting NcmMSet to configuration file: %s", error->message);
    if (!g_file_set_contents (filename, mset_data, len, &error))
      g_error ("Error saving configuration file to disk: %s", error->message);
    g_free (mset_data);
    g_key_file_free (msetfile);
  }
}

/**
 * ncm_mset_load:
 * @filename: mset filename
 * @ser: a #NcmSerialize
 * 
 * FIXME
 * 
 * Returns: (transfer full): FIXME
 */
NcmMSet * 
ncm_mset_load (const gchar *filename, NcmSerialize *ser)
{
  NcmMSet *mset = ncm_mset_empty_new ();
  GKeyFile *msetfile = g_key_file_new ();
  GError *error = NULL;
  gchar **groups = NULL;
  gsize ngroups = 0;
  gboolean valid_map = FALSE;
  guint i;
  
  if (!g_key_file_load_from_file (msetfile, filename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error))
  {
    g_error ("ncm_mset_load: Invalid mset configuration file: %s %s", filename, error->message);
    return NULL;
  }

  if (g_key_file_has_group (msetfile, "NcmMSet"))
  {
    if (g_key_file_has_key (msetfile, "NcmMSet", "valid_map", &error))
    {
      valid_map = g_key_file_get_boolean (msetfile, "NcmMSet", "valid_map", &error);
    }
    g_key_file_remove_group (msetfile, "NcmMSet", &error);
  }
  
  groups = g_key_file_get_groups (msetfile, &ngroups);
  for (i = 0; i < ngroups; i++)
  {
    GString *obj_ser = g_string_sized_new (200);    
    
    if (!g_key_file_has_key (msetfile, groups[i], groups[i], &error))
    {
      if (error != NULL)
        g_error ("ncm_mset_load: %s", error->message);
      g_error ("ncm_mset_load: Every group must contain a key with same name indicating the object type.");
    }

    {
      gchar *obj_type = g_key_file_get_value (msetfile, groups[i], groups[i], &error);
      g_string_append_printf (obj_ser, "{\'%s\', @a{sv} {", obj_type);
      g_free (obj_type);
      g_key_file_remove_key (msetfile, groups[i], groups[i], &error);
      if (error != NULL)
        g_error ("ncm_mset_load: %s", error->message);
    }

    {
      gsize nkeys = 0;
      gchar **keys = g_key_file_get_keys (msetfile, groups[i], &nkeys, &error);
      guint j;
      if (error != NULL)
        g_error ("ncm_mset_load: %s", error->message);
      for (j = 0; j < nkeys; j++)
      {
        gchar *propval = g_key_file_get_value (msetfile, groups[i], keys[j], &error);
        if (error != NULL)
          g_error ("ncm_mset_load: %s", error->message);
        g_string_append_printf (obj_ser, "\'%s\':<%s>", keys[j], propval);
        g_free (propval);
        if (j + 1 != nkeys)
          g_string_append (obj_ser, ", ");
      }
      g_string_append (obj_ser, "}}");
      g_strfreev (keys);
    }

    {
      GObject *obj = ncm_serialize_from_string (ser, obj_ser->str);
      g_assert (NCM_IS_MODEL (obj));
      if (ncm_mset_exists (mset, NCM_MODEL (obj)))
        g_error ("ncm_mset_load: a model ``%s'' already exists in NcmMSet.", obj_ser->str);
      ncm_mset_set (mset, NCM_MODEL (obj));
      ncm_model_free (NCM_MODEL (obj));
    }
    g_string_free (obj_ser, TRUE);
  }

  g_key_file_unref (msetfile);
  g_strfreev (groups);
  if (valid_map)
    ncm_mset_prepare_fparam_map (mset);

  return mset;
}

static void
ncm_mset_init (NcmMSet *mset)
{
  guint i;
  memset (mset->model, 0, sizeof (NcmModel *) * NCM_MODEL_MAX_ID);
  mset->pi_array = g_array_sized_new (FALSE, TRUE, sizeof (NcmMSetPIndex), 20);
  for (i = 0; i < NCM_MODEL_MAX_ID; i++)
    mset->fpi_array[i] = g_array_sized_new (FALSE, TRUE, sizeof (gint), 10);

  mset->fullname_parray = g_ptr_array_new ();
  g_ptr_array_set_free_func (mset->fullname_parray, g_free);

  mset->valid_map = FALSE;
  mset->total_len = 0;
}

static void
_ncm_mset_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  NcmMSet *mset = NCM_MSET (object);
  g_return_if_fail (NCM_IS_MSET (object));

  switch (prop_id)
  {
    case PROP_VALID_MAP:
      g_value_set_boolean (value, mset->valid_map);
      break;
    default:
    {
      g_assert (prop_id >= PROP_SIZE);
      if (prop_id >= PROP_SIZE + NCM_MODEL_MAX_ID)
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      else
      {
        guint mid = prop_id - PROP_SIZE;
        g_value_set_object (value, ncm_mset_peek (mset, mid));
      }
      break;
    }
  }
}

static void
_ncm_mset_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  NcmMSet *mset = NCM_MSET (object);
  g_return_if_fail (NCM_IS_MSET (object));

  switch (prop_id)
  {
    case PROP_VALID_MAP:
    {
      if (g_value_get_boolean (value))
      {
        if (!mset->valid_map)
          ncm_mset_prepare_fparam_map (mset);
      }
      else
        mset->valid_map = FALSE;
      break;
    }
    default:
    {
      g_assert (prop_id >= PROP_SIZE);
      if (prop_id >= PROP_SIZE + NCM_MODEL_MAX_ID)
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      else
      {
        guint mid = prop_id - PROP_SIZE;
        NcmModel *model = g_value_get_object (value);
        if (model != NULL)
          ncm_mset_set (mset, model);
        else
          ncm_mset_remove (mset, mid);
      }
      break;
    }
  }
}

static void
_ncm_mset_dispose (GObject *object)
{
  NcmMSet *mset = NCM_MSET (object);
  NcmModelID mid;

  for (mid = 0; mid < NCM_MODEL_MAX_ID; mid++)
  {
    if (mset->model[mid] != NULL)
      ncm_model_clear (&mset->model[mid]);
  }

  /* Chain up : end */
  G_OBJECT_CLASS (ncm_mset_parent_class)->dispose (object);
}

static void
_ncm_mset_finalize (GObject *object)
{
  NcmMSet *mset = NCM_MSET (object);
  gint i;

  for (i = 0; i < NCM_MODEL_MAX_ID; i++)
    g_array_unref (mset->fpi_array[i]);

  g_array_unref (mset->pi_array);

  g_ptr_array_unref (mset->fullname_parray);

  /* Chain up : end */
  G_OBJECT_CLASS (ncm_mset_parent_class)->finalize (object);
}

static void
ncm_mset_class_init (NcmMSetClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  guint i;

  object_class->set_property = &_ncm_mset_set_property;
  object_class->get_property = &_ncm_mset_get_property;
  object_class->dispose      = &_ncm_mset_dispose;
  object_class->finalize     = &_ncm_mset_finalize;

  klass->ns_table = g_hash_table_new (g_str_hash, g_str_equal);

  g_object_class_install_property (object_class, PROP_VALID_MAP,
                                   g_param_spec_boolean ("valid-map", NULL, "Valid properties map",
                                                        FALSE, 
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));

  for (i = 0; i < NCM_MODEL_MAX_ID; i++)
  {
    gchar *name = g_strdup_printf ("model-type-%u", i);
    gchar *desc = g_strdup_printf ("Model type %u", i);
    klass->model_desc[i].init = FALSE;
    g_object_class_install_property (object_class, PROP_SIZE + i,
                                     g_param_spec_object (name, NULL, desc,
                                                          NCM_TYPE_MODEL,
                                                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
    g_free (name);
    g_free (desc);
  }
}
