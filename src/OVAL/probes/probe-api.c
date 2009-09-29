#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "probe-api.h"

/*
This file contains functions for manipulating with the S-exp representation of OVAL objects and items.
Currently object and items have the same structure and the API distinction is just formal. However, the
structure can diverge in the future and the API functions for manipulating with items should be used
only with items and vice versa. The most recent description of the object and item structure can be found
in this file and should be used as the main source of information for implementing new API functions.

Object structure
----------------

The basic structure of an object is

   (foo_object bar)

where foo_object is the object's name and bar is the value of this object. The value is optional.
In the case the object has some attributes the structure is

   ((foo_object :attr1 val1) bar)

where attr1 is the name of the attribute and val1 is the attribute's value. The colon in the
attribute name signals that this attribute has a value. 

*/

/*
 * items
 */

SEXP_t *probe_item_build (const char *fmt, ...)
{
        /* TBI */
        return (NULL);
}

/*
  SEXP_t *probe_item_creat (const char *name, SEXP_t *attrs, ...)
  {
  return probe_obj_creat (name, attrs, ...);
  }
*/

SEXP_t *probe_item_new (const char *name, SEXP_t *attrs)
{
        /*
         * Objects have the same structure as items.
         */
        return probe_obj_new (name, attrs);
}

SEXP_t *probe_item_attr_add (SEXP_t *item, const char *name, SEXP_t *val)
{
        SEXP_t *n_ref, *ns;
        
        n_ref = SEXP_listref_first (item);
        
        if (SEXP_listp (n_ref)) {
                /*
                 * There are already some attributes.
                 * Just add the new to the list.
                 */
                if (val == NULL)
                        ns = SEXP_string_new (name, strlen (name));
                else
                        ns = SEXP_string_newf ("%:s", name);
                
                SEXP_list_add (n_ref, ns);
                
                if (val != NULL)
                        SEXP_list_add (n_ref, val);
        } else {
                /*
                 * There aren't attributes in this item.
                 * We need to replace the item name S-exp
                 * with a list containing the item name
                 * S-exp and the attribute.
                 */
                SEXP_t *nl;
                
                if (val == NULL)
                        ns = SEXP_string_new (name, strlen (name));
                else
                        ns = SEXP_string_newf (":%s", name);
                
                nl = SEXP_list_new (n_ref, ns, val, NULL);
                
                SEXP_free (n_ref);
                SEXP_free (ns);
                
                n_ref = SEXP_list_replace (item, 1, nl);
        }
        
        SEXP_free (n_ref);
        
        return (val);
}

SEXP_t *probe_item_ent_add ()
{
        /* TBI */
        return (NULL);
}

int probe_item_setstatus (SEXP_t *obj, oval_syschar_status_t status)
{
        /* TBI */
        _A(obj != NULL);
        
        
        
        return (-1);
}

int probe_itement_setstatus (SEXP_t *obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
        /* TBI */
        return (-1);
}

/*
 * attributes
 */

SEXP_t *probe_attr_creat (const char *name, const SEXP_t *val, ...)
{
        va_list ap;
        SEXP_t *list, *ns;
        
        va_start (ap, val);
        list = SEXP_list_new (NULL);
        
        while (name != NULL) {
                if (val == NULL) {
                        ns = SEXP_string_new (name, strlen (name));
                        SEXP_list_add (list, ns);
                        SEXP_free (ns);
                } else {
                        ns = SEXP_string_newf (":%s", name);
                        SEXP_list_add (list, ns);
                        SEXP_list_add (list, val);
                        SEXP_free (ns);
                }
                
                name = va_arg (ap, const char *);
                val  = va_arg (ap, SEXP_t *);
        }
        
        va_end (ap);

        return (list);
}

/*
 * objects
 */

SEXP_t *probe_obj_build (const char *fmt, ...)
{
        /* TBI */
        return (NULL);
}

SEXP_t *probe_obj_creat (const char *name, SEXP_t *attrs, ...)
{
        va_list ap;
        SEXP_t *obj, *ns, *val, *ent;

        obj  = probe_obj_new (name, attrs);
        name = va_arg (ap, const char *);
        
        while (name != NULL) {
                attrs = va_arg (ap, SEXP_t *);
                val   = va_arg (ap, SEXP_t *);

                ns  = SEXP_string_new (name, strlen (name));
                ent = SEXP_list_new (NULL);
                
                if (attrs != NULL) {
                        SEXP_t *nl;

                        nl = SEXP_list_new (ns);
                        nl = SEXP_list_join (nl, attrs);
                        
                        SEXP_list_add (ent, nl);
                        SEXP_free (nl);
                } else
                        SEXP_list_add (ent, ns);

                SEXP_free (ns);
                SEXP_list_add (ent, val);
                SEXP_list_add (obj, ent);
        }

        return (obj);
}

SEXP_t *probe_obj_new (const char *name, SEXP_t *attrs)
{
        SEXP_t *obj, *ns;
        
        obj = SEXP_list_new (NULL);
        ns  = SEXP_string_new (name, strlen (name));
        
        if (attrs != NULL) {
                SEXP_t *nl;
                
                nl = SEXP_list_new (ns);
                nl = SEXP_list_join (nl, attrs);
                
                SEXP_list_add (obj, nl);
                SEXP_free (nl);
        } else
                SEXP_list_add (obj, ns);
        
        SEXP_free (ns);
        
        return (obj);
}

SEXP_t *probe_obj_getent (const SEXP_t *obj, const char *name, uint32_t n)
{
        SEXP_t *objents, *ent, *ent_name;
        
        _A(obj  != NULL);
        _A(name != NULL);
        _A(n > 0);
        
        objents = SEXP_list_rest (obj);
        
        SEXP_list_foreach (ent, objents) {
                ent_name = SEXP_listref_first (ent);
                
                if (SEXP_listp (ent_name)) {
                        SEXP_t *nr;
                        
                        nr = SEXP_listref_first (ent_name);
                        SEXP_free (ent_name);
                        ent_name = nr;
                }
                
                if (SEXP_stringp (ent_name)) {
                        if (SEXP_strcmp (ent_name, name) == 0 && (--n == 0))
                                break;
                }
        }
        
        SEXP_free (objents);
        
        return (ent);
}

SEXP_t *probe_obj_getentval  (const SEXP_t *obj, const char *name, uint32_t n)
{
        /* TBI */
        return (NULL);
}

int probe_obj_getentvals (const SEXP_t *obj, const char *name, uint32_t n, SEXP_t **res)
{
        /* TBI */
        return (-1);
}

SEXP_t *probe_obj_getattrval (const SEXP_t *obj, const char *name)
{
        /* TBI */
        return (NULL);
}

bool probe_obj_attrexists (const SEXP_t *obj, const char *name)
{
        /* TBI */
        return (false);
}

int probe_obj_setstatus (SEXP_t *obj, oval_syschar_status_t status)
{
        /* TBI */
        return (-1);
}

int probe_obj_setentstatus (SEXP_t *obj, const char *name, uint32_t n, oval_syschar_status_t status)
{
        /* TBI */
        return (-1);
}

/*
 * entities
 */

SEXP_t *probe_ent_creat (const char *name, SEXP_t *attrs, ...)
{
        /* TBI */
        return (NULL);
}

SEXP_t *probe_ent_attr_add (SEXP_t *ent, const char *name, SEXP_t *val)
{
        /* TBI */
        return (NULL);
}

int probe_ent_getvals (const SEXP_t *ent, SEXP_t **res)
{
        (*res) = SEXP_list_rest (ent);
        return (SEXP_list_length (*res));
}

SEXP_t *probe_ent_getval (const SEXP_t *ent)
{
        return (SEXP_list_nth (ent, 2));
}

SEXP_t *probe_ent_getattrval (const SEXP_t *ent, const char *name)
{
        SEXP_t *attrs;

        if (ent == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        attrs = SEXP_list_first (ent);

        if (SEXP_listp (attrs)) {
                SEXP_t  *attr;
                uint32_t i;

                i = 2;

                while ((attr = SEXP_list_nth (attrs, i)) != NULL) {
                        if (SEXP_stringp (attr)) {
                                char   attr_name[32+1];
                                size_t attr_nlen;

                                attr_nlen = SEXP_string_cstr_r (attr, attr_name, sizeof attr_name);
                                
                                if (attr_nlen > 2) {
                                        if (attr_name[0] == ':') {
                                                if (strcmp (attr_name + 1, name) == 0) {
                                                        SEXP_free (attr);
                                                        return SEXP_list_nth (attrs, i + 1);
                                                }
                                        }
                                }
                                
                                SEXP_free (attr);
                        }
                        
                        ++i;
                }
        }
        
        return (NULL);
}

bool probe_ent_attrexists (const SEXP_t *ent, const char *name)
{
        /* TBI */
        return (false);
}

int probe_ent_setdatatype (SEXP_t *ent, oval_datatype_t type)
{
        SEXP_t *val;
        
        _A(ent != NULL);

        val = probe_ent_getval (ent);
        
        if (val == NULL)
                return (-1);
        
	switch (type) {
        case OVAL_DATATYPE_BINARY:
                return SEXP_datatype_set (val, "binary");
        case OVAL_DATATYPE_BOOLEAN:
                return SEXP_datatype_set (val, "bool");
        case OVAL_DATATYPE_EVR_STRING:
                return SEXP_datatype_set (val, "evr_str");
        case OVAL_DATATYPE_FILESET_REVISTION:
                return SEXP_datatype_set (val, "fset_rev");
        case OVAL_DATATYPE_FLOAT:
                /* TODO */
                return (-1);
        case OVAL_DATATYPE_IOS_VERSION:
                return SEXP_datatype_set (val, "ios_ver");
        case OVAL_DATATYPE_VERSION:
                return SEXP_datatype_set (val, "version");
        case OVAL_DATATYPE_INTEGER:
                /* TODO */
                return (-1);
        case OVAL_DATATYPE_STRING:
                return (SEXP_stringp (val) ? 0 : -1);
        default:
                return (-1);
        }

        return (0);
}

oval_datatype_t probe_ent_getdatatype (const SEXP_t *ent)
{
        SEXP_t     *val;
        const char *str;
        
        _A(ent != NULL);

        val = probe_ent_getval (ent);

        if (val == NULL)
                return (-1);

        str = SEXP_datatype (val);
        
        if (str != NULL) {
                switch (str[0]) {
                case 'b':
                        if (strcmp (str, "bool") == 0)
                                return (OVAL_DATATYPE_BOOLEAN);
                        if (strcmp (str, "binary") == 0)
                                return (OVAL_DATATYPE_BINARY);
                        break;
                case 'e':
                        if (strcmp (str, "evr_str") == 0)
                                return (OVAL_DATATYPE_EVR_STRING);
                        break;
                case 'f':
                        /* FIXME: typo in oval_definitions.h?
                           if (strcmp (str, "fset_rev") == 0)
                           return (OVAL_DATYPE_FILESET_REVISION);
                        */
                        break;
                case 'i':
                        if (strcmp (str, "ios_ver") == 0)
                                return (OVAL_DATATYPE_IOS_VERSION);
                        break;
                case 'v':
                        if (strcmp (str, "version") == 0)
                                return (OVAL_DATATYPE_VERSION);
                        break;
                }
        } else {
                switch (SEXP_typeof (val)) {
                case SEXP_TYPE_NUMBER:
                        /* FIXME: float vs. integer */
                        return (OVAL_DATATYPE_INTEGER);
                case SEXP_TYPE_STRING:
                        return (OVAL_DATATYPE_STRING);
                }
        }
        
        return (OVAL_DATATYPE_UNKNOWN);
}

int probe_ent_setmask (SEXP_t *ent, bool mask)
{
        /* TBI */
        return (-1);
}

bool probe_ent_getmask (const SEXP_t *ent)
{
        /* TBI */
        return (false);
}

int probe_ent_setstatus (SEXP_t *ent, oval_syschar_status_t status)
{
        /* TBI */
        return (-1);
}

oval_syschar_status_t probe_ent_getstatus (const SEXP_t *ent)
{
        /* TBI */
        return (-1);
}

char *probe_ent_getname (const SEXP_t *ent)
{
        SEXP_t *ent_name;
        char   *name_str;
        
        if (ent == NULL) {
                errno = EFAULT;
                return (NULL);
        }

        name_str = NULL;        
        ent_name = SEXP_list_first (ent);

        if (ent_name == NULL) {
                errno = EINVAL;
                return (NULL);
        }

        switch (SEXP_typeof (ent_name)) {
        case SEXP_TYPE_LIST:
        {
                SEXP_t *tmp;
                
                tmp = SEXP_list_first (ent_name);
                SEXP_free (ent_name);
                ent_name = tmp;
                
                if (!SEXP_stringp (ent_name)) {
                        errno = EINVAL;
                        break;
                }
        }
        case SEXP_TYPE_STRING:
                if (SEXP_string_length (ent_name) > 0)
                        name_str = SEXP_string_cstr (ent_name);
                else
                        errno = EINVAL;
        }
        
        SEXP_free (ent_name);

        return (name_str);
}

char *probe_ent_getname_r (const SEXP_t *ent, char *buffer, size_t buflen)
{
        /* TBI */
        abort ();
        return (NULL);
}
