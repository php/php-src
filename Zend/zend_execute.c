/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <signal.h>

#include "zend.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_ptr_stack.h"
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_extensions.h"


static void destroy_garbage(HashTable *ht)
{
	zend_hash_destroy(ht);
	efree(ht);
}


#define INC_AI_COUNT(znode_ptr)		if (!((znode_ptr)->u.EA.type & EXT_TYPE_UNUSED)) { EG(AiCount)++; }
#define DEC_AI_COUNT()		if (--EG(AiCount)==0) { zend_ptr_stack_clean(&EG(garbage), (void (*)(void *)) destroy_garbage); }

#define get_zval_ptr(node, Ts, should_free, type) _get_zval_ptr(node, Ts, should_free ELS_CC)
#define get_zval_ptr_ptr(node, Ts, type) _get_zval_ptr_ptr(node, Ts ELS_CC)

#define get_incdec_op(op, opcode) \
	switch (opcode) { \
		case ZEND_PRE_INC: \
		case ZEND_POST_INC: \
			(op) = increment_function; \
			break; \
		case ZEND_PRE_DEC: \
		case ZEND_POST_DEC: \
			(op) = decrement_function; \
			break; \
		default: \
			(op) = NULL; \
			break; \
	} \

/* These globals don't have to be thread safe since they're never modified */


/* Prototypes */
static zval get_overloaded_property(ELS_D);
static void set_overloaded_property(zval *value ELS_DC);
static void call_overloaded_function(HashTable *ht, zval *return_value, HashTable *list, HashTable *plist ELS_DC);
static inline void zend_fetch_var_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC);
static inline void zend_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC);
static inline void zend_fetch_property_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC);
static inline void zend_fetch_dimension_address_from_tmp_var(znode *result, znode *op1, znode *op2, temp_variable *Ts ELS_DC);
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array);
static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array);
static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array);


static inline zval *_get_zval_ptr(znode *node, temp_variable *Ts, int *should_free ELS_DC)
{
	switch(node->op_type) {
		case IS_CONST:
			*should_free = 0;
			return &node->u.constant;
			break;
		case IS_TMP_VAR:
			*should_free = 1;
			return &Ts[node->u.var].tmp_var;
			break;
		case IS_VAR:
			if (Ts[node->u.var].var) {
				DEC_AI_COUNT();
				*should_free = 0;
				return *Ts[node->u.var].var;
			} else {
				*should_free = 1;

				switch (Ts[node->u.var].EA.type) {
					case IS_OVERLOADED_OBJECT:
						Ts[node->u.var].tmp_var = get_overloaded_property(ELS_C);
						Ts[node->u.var].tmp_var.refcount=1;
						Ts[node->u.var].tmp_var.is_ref=1;
						return &Ts[node->u.var].tmp_var;
						break;
					case IS_STRING_OFFSET: {
							temp_variable *T = &Ts[node->u.var];
							zval *str = T->EA.str;

							if (T->EA.str->type != IS_STRING
								|| (T->EA.str->value.str.len <= T->EA.offset)) {
								T->tmp_var.value.str.val = empty_string;
								T->tmp_var.value.str.len = 0;
							} else {
								char c = str->value.str.val[T->EA.offset];

								T->tmp_var.value.str.val = estrndup(&c, 1);
								T->tmp_var.value.str.len = 1;
							}
							zval_ptr_dtor(&str);
							T->tmp_var.refcount=1;
							T->tmp_var.is_ref=1;
							T->tmp_var.type = IS_STRING;
							return &T->tmp_var;
						}
						break;
				}
			}
			break;
		case IS_UNUSED:
			return NULL;
			break;
#if DEBUG_ZEND
		default:
			zend_error(E_ERROR, "Unknown temporary variable type");
			break;
#endif
	}
	return NULL;
}


static inline zval **_get_zval_ptr_ptr(znode *node, temp_variable *Ts ELS_DC)
{
	switch(node->op_type) {
		case IS_VAR:
			if (Ts[node->u.var].var) {
				DEC_AI_COUNT();
			}
			return Ts[node->u.var].var;
			break;
		default:
			return NULL;
			break;
	}
}


static inline zval **zend_fetch_property_address_inner(HashTable *ht, znode *op2, temp_variable *Ts, int type ELS_DC)
{
	int free_op2;
	zval *prop_ptr = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);
	zval **retval;
	zval tmp;


	switch (op2->op_type) {
		case IS_CONST:
			/* already a constant string */
			break;
		case IS_VAR:
			tmp = *prop_ptr;
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
			prop_ptr = &tmp;
			break;
		case IS_TMP_VAR:
			convert_to_string(prop_ptr);
			break;
	}

	if (zend_hash_find(ht, prop_ptr->value.str.val, prop_ptr->value.str.len+1, (void **) &retval) == FAILURE) {
		switch (type) {
			case BP_VAR_R: 
				zend_error(E_NOTICE,"Undefined property:  %s", prop_ptr->value.str.val);
				/* break missing intentionally */
			case BP_VAR_IS:
				retval = &EG(uninitialized_zval_ptr);
				break;
			case BP_VAR_RW:
				zend_error(E_NOTICE,"Undefined property:  %s", prop_ptr->value.str.val);
				/* break missing intentionally */
			case BP_VAR_W: {
					zval *new_zval = (zval *) emalloc(sizeof(zval));

					var_uninit(new_zval);
					new_zval->refcount=1;
					new_zval->is_ref=0;
					//zend_hash_update(ht, prop_ptr->value.str.val, prop_ptr->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					zend_hash_update_ptr(ht, prop_ptr->value.str.val, prop_ptr->value.str.len+1, new_zval, sizeof(zval *), (void **) &retval);
				}
				break;
		}
	}

	if (prop_ptr == &tmp) {
		zval_dtor(prop_ptr);
	}
	FREE_OP(op2, free_op2);
	return retval;
}


static inline void zend_assign_to_variable(znode *result, znode *op1, zval *value, int type, temp_variable *Ts ELS_DC)
{
	zval **variable_ptr_ptr = get_zval_ptr_ptr(op1, Ts, BP_VAR_W);
	zval *variable_ptr;

	if (!variable_ptr_ptr) {
		switch (Ts[op1->u.var].EA.type) {
			case IS_OVERLOADED_OBJECT:
				set_overloaded_property(value ELS_CC);
				if (type == IS_TMP_VAR) {
					zval_dtor(value);
				}
				break;
			case IS_STRING_OFFSET: {
					temp_variable *T = &Ts[op1->u.var];

					if (T->EA.str->type == IS_STRING
						&& (T->EA.offset < T->EA.str->value.str.len)) {
						zval tmp;

						if (value->type!=IS_STRING) {
							tmp = *value;
							zval_copy_ctor(&tmp);
							convert_to_string(&tmp);
							value = &tmp;
						}

						T->EA.str->value.str.val[T->EA.offset] = value->value.str.val[0];
						if (value == &tmp) {
							zval_dtor(value);
						}
						/*
						 * the value of an assignment to a string offset is undefined
						Ts[result->u.var].var = &T->EA.str;
						*/
					}
					zval_ptr_dtor(&T->EA.str);
					T->tmp_var.type = IS_STRING;
				}
				break;
		}
		Ts[result->u.var].var = &EG(uninitialized_zval_ptr);
		INC_AI_COUNT(result);
		return;
	}

	variable_ptr = *variable_ptr_ptr;

	if (variable_ptr == EG(error_zval_ptr)) {
		if (result) {
			Ts[result->u.var].var = &EG(uninitialized_zval_ptr);
			INC_AI_COUNT(result);
		}
		return;
	}
	
	if (variable_ptr->is_ref) {
		if (variable_ptr!=value) {
			short refcount=variable_ptr->refcount;
	
			zendi_zval_dtor(*variable_ptr);
			*variable_ptr = *value;
			variable_ptr->refcount = refcount;
			variable_ptr->is_ref=1;
			if (type!=IS_TMP_VAR) {
				zendi_zval_copy_ctor(*variable_ptr);
			}
		}
	} else {
		variable_ptr->refcount--;
		variable_ptr = *variable_ptr_ptr;
		if (variable_ptr->refcount==0) {
			switch (type) {
				case IS_VAR:
				case IS_CONST:
					if (variable_ptr==value) {
						variable_ptr->refcount++;
					} else if (value->is_ref) {
						*variable_ptr = *value;
						zval_copy_ctor(variable_ptr);
						variable_ptr->refcount=1;
					} else {
						zendi_zval_dtor(*variable_ptr);
						safe_free_zval_ptr(variable_ptr);
						*variable_ptr_ptr = value;
						value->refcount++;
					}
					break;
				case IS_TMP_VAR:
					zendi_zval_dtor(*variable_ptr);
					value->refcount=1;
					*variable_ptr = *value;
					break;
			}
		} else { /* we need to split */
			switch (type) {
				case IS_VAR:
				case IS_CONST:
					if (value->is_ref) {
						variable_ptr = *variable_ptr_ptr = (zval *) emalloc(sizeof(zval));
						*variable_ptr = *value;
						zval_copy_ctor(variable_ptr);
						variable_ptr->refcount=1;
						break;
					}
					*variable_ptr_ptr = value;
					value->refcount++;
					break;
				case IS_TMP_VAR:
					(*variable_ptr_ptr) = (zval *) emalloc(sizeof(zval));
					value->refcount=1;
					**variable_ptr_ptr = *value;
					break;
			}
		}
		(*variable_ptr_ptr)->is_ref=0;
	}
	if (result) {
		Ts[result->u.var].var = variable_ptr_ptr;
		INC_AI_COUNT(result);
	}
}


/* Utility Functions for Extensions */
static void zend_extension_statement_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->statement_handler) {
		extension->statement_handler(op_array);
	}
}


static void zend_extension_fcall_begin_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->fcall_begin_handler) {
		extension->fcall_begin_handler(op_array);
	}
}


static void zend_extension_fcall_end_handler(zend_extension *extension, zend_op_array *op_array)
{
	if (extension->fcall_end_handler) {
		extension->fcall_end_handler(op_array);
	}
}


static void print_refcount(zval *p, char *str)
{
	//printf("*** refcount for %x - '%s':  %d\n", p, str, p->refcount);
	print_refcount(NULL, NULL);
}


static inline void zend_fetch_var_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC)
{
	int free_op1;
	zval *varname = get_zval_ptr(op1, Ts, &free_op1, BP_VAR_R);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

	switch (op2->u.constant.value.lval) {
		case ZEND_FETCH_LOCAL:
		default: /* just to shut gcc up */
			target_symbol_table = EG(active_symbol_table);
			break;
		case ZEND_FETCH_GLOBAL:
			target_symbol_table = &EG(symbol_table);
			break;
		case ZEND_FETCH_STATIC:
			if (!EG(active_op_array)->static_variables) {
				EG(active_op_array)->static_variables = (HashTable *) emalloc(sizeof(HashTable));
				zend_hash_init(EG(active_op_array)->static_variables, 2, NULL, PVAL_PTR_DTOR, 0);
			}
			target_symbol_table = EG(active_op_array)->static_variables;
			break;
	}

	if (varname->type != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}
	if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
		switch (type) {
			case BP_VAR_R: 
				zend_error(E_NOTICE,"Undefined variable:  %s", varname->value.str.val);
				/* break missing intentionally */
			case BP_VAR_IS:
				retval = &EG(uninitialized_zval_ptr);
				break;
			case BP_VAR_RW:
				zend_error(E_NOTICE,"Undefined variable:  %s", varname->value.str.val);
				/* break missing intentionally */
			case BP_VAR_W: {
					zval *new_zval = (zval *) emalloc(sizeof(zval));

					var_uninit(new_zval);
					new_zval->refcount=1;
					new_zval->is_ref=0;
					//zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					zend_hash_update_ptr(target_symbol_table, varname->value.str.val, varname->value.str.len+1, new_zval, sizeof(zval *), (void **) &retval);
				}
				break;
		}
	}
	if (op2->u.constant.value.lval == ZEND_FETCH_LOCAL) {
		FREE_OP(op1, free_op1);
	} else if (op2->u.constant.value.lval == ZEND_FETCH_STATIC) {
		zval_update_constant(*retval);
	}

	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	Ts[result->u.var].var = retval;	
	INC_AI_COUNT(result);
}


static inline zval **zend_fetch_dimension_address_inner(HashTable *ht, znode *op2, temp_variable *Ts, int type ELS_DC)
{
	int free_op2;
	zval *dim = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);
	zval **retval;

	switch (dim->type) {
		case IS_STRING: {
				if (zend_hash_find(ht, dim->value.str.val, dim->value.str.len+1, (void **) &retval) == FAILURE) {
					switch (type) {
						case BP_VAR_R: 
							zend_error(E_NOTICE,"Undefined index:  %s", dim->value.str.val);
							/* break missing intentionally */
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval_ptr);
							break;
						case BP_VAR_RW:
							zend_error(E_NOTICE,"Undefined index:  %s", dim->value.str.val);
							/* break missing intentionally */
						case BP_VAR_W: {
								zval *new_zval = (zval *) emalloc(sizeof(zval));

								var_uninit(new_zval);
								new_zval->refcount=1;
								new_zval->is_ref=0;
								//zend_hash_update(ht, dim->value.str.val, dim->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
								zend_hash_update_ptr(ht, dim->value.str.val, dim->value.str.len+1, new_zval, sizeof(zval *), (void **) &retval);
							}
							break;
					}
				}
			}
			break;
		case IS_LONG: {
				if (zend_hash_index_find(ht, dim->value.lval, (void **) &retval) == FAILURE) {
					switch (type) {
						case BP_VAR_R: 
							zend_error(E_NOTICE,"Undefined offset:  %d", dim->value.lval);
							/* break missing intentionally */
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval_ptr);
							break;
						case BP_VAR_RW:
							zend_error(E_NOTICE,"Undefined offset:  %d", dim->value.lval);
							/* break missing intentionally */
						case BP_VAR_W: {
								zval *new_zval = (zval *) emalloc(sizeof(zval));

								var_uninit(new_zval);
								new_zval->refcount=1;
								new_zval->is_ref=0;
								zend_hash_index_update(ht, dim->value.lval, &new_zval, sizeof(zval *), (void **) &retval);
							}
							break;
					}
				}
			}
			break;
		/* we need to do implement this nicely somehow ZA
		case IS_DOUBLE:
			break;
		*/
		default: 
			zend_error(E_WARNING, "Illegal offset type");
			if (type == BP_VAR_R || type == BP_VAR_IS) {
				retval = &EG(uninitialized_zval_ptr);
			} else {
				retval = &EG(error_zval_ptr);
			}
			break;
	}
	FREE_OP(op2, free_op2);
	return retval;
}


static inline void zend_fetch_dimension_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC)
{
	int free_op2;
	zval **container_ptr = get_zval_ptr_ptr(op1, Ts, type);
	zval *container;
	zval ***retval = &Ts[result->u.var].var;


	if (container_ptr == NULL) {
		zend_property_reference *property_reference;
		zend_overloaded_element overloaded_element;

		if (Ts[op1->u.var].EA.type == IS_STRING_OFFSET) {
			zval_ptr_dtor(&Ts[op1->u.var].EA.str);
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_IS:
					*retval = &EG(uninitialized_zval_ptr);
					return;
				case BP_VAR_W:
				case BP_VAR_RW:
					*retval = &EG(error_zval_ptr);
					return;
			}
		}

		/* prepare the new element */
		overloaded_element.element = *get_zval_ptr(op2, Ts, &free_op2, type);
		overloaded_element.type = IS_ARRAY;
		if (!free_op2) {
			zval_copy_ctor(&overloaded_element.element);
		}

		zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);

		zend_llist_add_element(&property_reference->elements_list, &overloaded_element);

		Ts[result->u.var].EA.type = IS_OVERLOADED_OBJECT;
		*retval = NULL;
		return;
	}
	container = *container_ptr;

	if (container == EG(error_zval_ptr)) {
		INC_AI_COUNT(result);
		*retval = &EG(error_zval_ptr);
		return;
	}

	if (container->type == IS_STRING && container->value.str.val == undefined_variable_string) {
		switch (type) {
			case BP_VAR_RW:
			case BP_VAR_W:
				if (!container->is_ref) {
					container->refcount--;
					if (container->refcount>0) {
						container = *container_ptr = (zval *) emalloc(sizeof(zval));
						container->is_ref=0;
					}
					container->refcount=1;
				}
				array_init(container);
				break;
		}
	}

	switch (container->type) {
		case IS_ARRAY:
			if ((type==BP_VAR_W || type==BP_VAR_RW) && container->refcount>1 && !container->is_ref) {
				container->refcount--;
				*container_ptr = (zval *) emalloc(sizeof(zval));
				**container_ptr = *container;
				container = *container_ptr;
				container->refcount=1;
				container->is_ref=0;
				zendi_zval_copy_ctor(*container);
			}
			if (op2->op_type == IS_UNUSED) {
				zval *new_zval = (zval *) emalloc(sizeof(zval));

				var_uninit(new_zval);
				new_zval->refcount = 1;
				new_zval->is_ref = 0;
				zend_hash_next_index_insert_ptr(container->value.ht, new_zval, sizeof(zval *), (void **) retval);
			} else {
				*retval = zend_fetch_dimension_address_inner(container->value.ht, op2, Ts, type ELS_CC);
			}
			break;
		case IS_STRING: {
				zval *offset;
				offset = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);

				if (container->value.str.val == undefined_variable_string 
					|| type==BP_VAR_IS) {
					/* for read-mode only */
					*retval = &EG(uninitialized_zval_ptr);
					FREE_OP(op2, free_op2);
				} else {
					zval tmp;

					if (offset->type != IS_LONG) {
						tmp = *offset;
						zval_copy_ctor(&tmp);
						convert_to_long(&tmp);
						offset = &tmp;
					}
					Ts[result->u.var].EA.str = container;
					container->refcount++;
					Ts[result->u.var].EA.offset = offset->value.lval;
					Ts[result->u.var].EA.type = IS_STRING_OFFSET;
					FREE_OP(op2, free_op2);
					*retval = NULL;
					return;
				}
			}
			break;
		default: {
				zval *offset;

				offset = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);
				if (type==BP_VAR_R || type==BP_VAR_IS) {
					*retval = &EG(uninitialized_zval_ptr);
				} else {
					*retval = &EG(error_zval_ptr);
				}
				FREE_OP(op2, free_op2);
			}
			break;
	}
	INC_AI_COUNT(result);
}


static inline void zend_fetch_dimension_address_from_tmp_var(znode *result, znode *op1, znode *op2, temp_variable *Ts ELS_DC)
{
	int free_op1;
	zval *container = get_zval_ptr(op1, Ts, &free_op1, BP_VAR_R);

	if (container->type != IS_ARRAY) {
		INC_AI_COUNT(result);
		Ts[result->u.var].var = &EG(uninitialized_zval_ptr);
		return;
	}

	INC_AI_COUNT(result);
	Ts[result->u.var].var = zend_fetch_dimension_address_inner(container->value.ht, op2, Ts, BP_VAR_R ELS_CC);
}


static inline void zend_fetch_property_address(znode *result, znode *op1, znode *op2, temp_variable *Ts, int type ELS_DC)
{
	int free_op2;
	zval **container_ptr = get_zval_ptr_ptr(op1, Ts, type);
	zval *container;
	zval ***retval = &Ts[result->u.var].var;


	if (container_ptr == NULL) {
		zend_property_reference *property_reference;
		zend_overloaded_element overloaded_element;

		if (Ts[op1->u.var].EA.type == IS_STRING_OFFSET) {
			zval_ptr_dtor(&Ts[op1->u.var].EA.str);
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_IS:
					*retval = &EG(uninitialized_zval_ptr);
					return;
				case BP_VAR_W:
				case BP_VAR_RW:
					*retval = &EG(error_zval_ptr);
					return;
			}
		}

		overloaded_element.element = *get_zval_ptr(op2, Ts, &free_op2, type);
		overloaded_element.type = IS_OBJECT;
		if (!free_op2) {
			zval_copy_ctor(&overloaded_element.element);
		}

		zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);

		zend_llist_add_element(&property_reference->elements_list, &overloaded_element);

		Ts[result->u.var].EA.type = IS_OVERLOADED_OBJECT;
		*retval = NULL;
		return;
	}

	container = *container_ptr;
	if (container == EG(error_zval_ptr)) {
		INC_AI_COUNT(result);
		*retval = &EG(error_zval_ptr);
		return;
	}

	if (container->type == IS_OBJECT
		&& container->value.obj.ce->handle_property_get) {
		zend_property_reference property_reference;
		zend_overloaded_element overloaded_element;

		property_reference.object = container_ptr;
		property_reference.type = type;
		zend_llist_init(&property_reference.elements_list, sizeof(zend_overloaded_element), NULL, 0);
		overloaded_element.element = *get_zval_ptr(op2, Ts, &free_op2, type);
		overloaded_element.type = IS_OBJECT;
		if (!free_op2) {
			zval_copy_ctor(&overloaded_element.element);
		}
		zend_llist_add_element(&property_reference.elements_list, &overloaded_element);
		zend_stack_push(&EG(overloaded_objects_stack), &property_reference, sizeof(zend_property_reference));
		Ts[result->u.var].EA.type = IS_OVERLOADED_OBJECT;
		*retval = NULL;
		return;
	}



	if (container->type == IS_STRING && container->value.str.val == undefined_variable_string) {
		switch (type) {
			case BP_VAR_RW:
			case BP_VAR_W:
				if (!container->is_ref) {
					container->refcount--;
					if (container->refcount>0) {
						container = *container_ptr = (zval *) emalloc(sizeof(zval));
						container->is_ref=0;
					}
					container->refcount=1;
				}
				object_init(container);
				break;
		}
	}
		
	if (container->type != IS_OBJECT) {
		zval *offset;

		offset = get_zval_ptr(op2, Ts, &free_op2, BP_VAR_R);
		FREE_OP(op2, free_op2);
		INC_AI_COUNT(result);
		if (type==BP_VAR_R || type==BP_VAR_IS) {
			*retval = &EG(uninitialized_zval_ptr);
			return;
		} else {
			*retval = &EG(error_zval_ptr);
			return;
		}
	}


	if ((type==BP_VAR_W || type==BP_VAR_RW) && container->refcount>1 && !container->is_ref) {
		container->refcount--;
		*container_ptr = (zval *) emalloc(sizeof(zval));
		**container_ptr = *container;
		container = *container_ptr;
		container->refcount=1;
		container->is_ref=0;
		zendi_zval_copy_ctor(*container);
	}
	INC_AI_COUNT(result);
	*retval = zend_fetch_property_address_inner(container->value.obj.properties, op2, Ts, type ELS_CC);
}


static zval get_overloaded_property(ELS_D)
{
	zend_property_reference *property_reference;
	zval result;

	zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);
	result = (*(property_reference->object))->value.obj.ce->handle_property_get(property_reference);

	zend_llist_destroy(&property_reference->elements_list);

	zend_stack_del_top(&EG(overloaded_objects_stack));
	return result;
}


static void set_overloaded_property(zval *value ELS_DC)
{
	zend_property_reference *property_reference;

	zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);
	(*(property_reference->object))->value.obj.ce->handle_property_set(property_reference, value);

	zend_llist_destroy(&property_reference->elements_list);

	zend_stack_del_top(&EG(overloaded_objects_stack));
}


static void call_overloaded_function(HashTable *ht, zval *return_value, HashTable *list, HashTable *plist ELS_DC)
{
	zend_property_reference *property_reference;

	zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);
	(*(property_reference->object))->value.obj.ce->handle_function_call(ht, return_value, list, plist, property_reference);
	//(*(property_reference->object))->value.obj.ce->handle_function_call(NULL, NULL, NULL, NULL, NULL);
	zend_llist_destroy(&property_reference->elements_list);

	zend_stack_del_top(&EG(overloaded_objects_stack));
}


#if HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2)
#	define free_alloca(p)
#else
#	define alloca(p)		emalloc(p)
#	define free_alloca(p)	efree(p)
#endif


void execute(zend_op_array *op_array ELS_DC)
{
	zend_op *opline = op_array->opcodes;
	int free_op1, free_op2;
	int (*unary_op)(zval *result, zval *op1);
	int (*binary_op)(zval *result, zval *op1, zval *op2);
	zend_op *end = op_array->opcodes + op_array->last;
	zend_function_state function_state;
	HashTable *calling_symbol_table;
	zend_function *function_being_called=NULL;
	zval **object_ptr=NULL;
#if !defined (__GNUC__) || __GNUC__ < 2
	temp_variable *Ts = (temp_variable *) alloca(sizeof(temp_variable)*op_array->T);
#else
	temp_variable Ts[op_array->T];
#endif

#if SUPPORT_INTERACTIVE
	if (EG(interactive)) {
		opline = op_array->opcodes + op_array->start_op_number;
		end = op_array->opcodes + op_array->end_op_number;
	}
#endif

	EG(opline_ptr) = &opline;

	function_state.function = (zend_function *) op_array;
	EG(function_state_ptr) = &function_state;
#if ZEND_DEBUG
	/* function_state.function_symbol_table is saved as-is to a stack,
	 * which is an intentional UMR.  Shut it up if we're in DEBUG.
	 */
	function_state.function_symbol_table = NULL;
#endif

	while (opline<end) {
		switch(opline->opcode) {
			case ZEND_ADD:
				binary_op = add_function;
				goto binary_op_addr;
			case ZEND_SUB:
				binary_op = sub_function;
				goto binary_op_addr;
			case ZEND_MUL:
				binary_op = mul_function;
				goto binary_op_addr;
			case ZEND_DIV:
				binary_op = div_function;
				goto binary_op_addr;
			case ZEND_MOD:
				binary_op = mod_function;
				goto binary_op_addr;
			case ZEND_SL:
				binary_op = shift_left_function;
				goto binary_op_addr;
			case ZEND_SR:
				binary_op = shift_right_function;
				goto binary_op_addr;
			case ZEND_CONCAT:
				binary_op = concat_function;
				goto binary_op_addr;
			case ZEND_IS_EQUAL:
				binary_op = is_equal_function;
				goto binary_op_addr;
			case ZEND_IS_NOT_EQUAL:
				binary_op = is_not_equal_function;
				goto binary_op_addr;
			case ZEND_IS_SMALLER:
				binary_op = is_smaller_function;
				goto binary_op_addr;
			case ZEND_IS_SMALLER_OR_EQUAL:
				binary_op = is_smaller_or_equal_function;
				goto binary_op_addr;
			case ZEND_BW_OR:
				binary_op = bitwise_or_function;
				goto binary_op_addr;
			case ZEND_BW_AND:
				binary_op = bitwise_and_function;
				goto binary_op_addr;
			case ZEND_BW_XOR:
				binary_op = bitwise_xor_function;
				goto binary_op_addr;
			case ZEND_BOOL_XOR:
				binary_op = boolean_xor_function;
		  	    /* Fall through */
binary_op_addr:
				binary_op(&Ts[opline->result.u.var].tmp_var, 
							 get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R),
							 get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R) );
				FREE_OP(&opline->op1, free_op1);
				FREE_OP(&opline->op2, free_op2);
				break;
			case ZEND_BW_NOT:
			case ZEND_BOOL_NOT:
				unary_op = get_unary_op(opline->opcode);
				unary_op(&Ts[opline->result.u.var].tmp_var,
							get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R) );
				FREE_OP(&opline->op1, free_op1);
				break;

			case ZEND_ASSIGN_ADD:
				binary_op = add_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_SUB:
				binary_op = sub_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_MUL:
				binary_op = mul_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_DIV:
				binary_op = div_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_MOD:
				binary_op = mod_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_SL:
				binary_op = shift_left_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_SR:
				binary_op = shift_right_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_CONCAT:
				binary_op = concat_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_BW_OR:
				binary_op = bitwise_or_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_BW_AND:
				binary_op = bitwise_and_function;
				goto binary_assign_op_addr;
			case ZEND_ASSIGN_BW_XOR:
				binary_op = bitwise_xor_function;
				/* Fall through */
binary_assign_op_addr: {
					zval **var_ptr = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_RW);
				
					if (!var_ptr) {
						zend_error(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
					}
					if (*var_ptr == EG(error_zval_ptr)) {
						Ts[opline->result.u.var].var = &EG(uninitialized_zval_ptr);
						INC_AI_COUNT(&opline->result);
						opline++;
						continue;
					}
					if (!(*var_ptr)->is_ref) {
						if ((*var_ptr)->refcount>1) {
							zval *orig_var=*var_ptr;
							
							(*var_ptr)->refcount--;
							*var_ptr = (zval *) emalloc(sizeof(zval));
							**var_ptr = *orig_var;
							zendi_zval_copy_ctor(**var_ptr);
							(*var_ptr)->refcount=1;
						}
					}
					binary_op(*var_ptr, *var_ptr, get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R));
					Ts[opline->result.u.var].var = var_ptr;
					INC_AI_COUNT(&opline->result);
					FREE_OP(&opline->op2, free_op2);
				}
				break;
			case ZEND_PRE_INC:
			case ZEND_PRE_DEC:
			case ZEND_POST_INC:
			case ZEND_POST_DEC: {
					int (*incdec_op)(zval *op);
					zval **var_ptr = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_RW);

					if (!var_ptr) {
						zend_error(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
					}
					if (*var_ptr == EG(error_zval_ptr)) {
						Ts[opline->result.u.var].var = &EG(uninitialized_zval_ptr);
						INC_AI_COUNT(&opline->result);
						opline++;
						continue;
					}

					get_incdec_op(incdec_op, opline->opcode);

					switch (opline->opcode) {
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
							Ts[opline->result.u.var].tmp_var = **var_ptr;
							zendi_zval_copy_ctor(Ts[opline->result.u.var].tmp_var);
							break;
					}
					if (!(*var_ptr)->is_ref) {
						if ((*var_ptr)->refcount>1) {
							zval *orig_var = *var_ptr;
							
							(*var_ptr)->refcount--;
							*var_ptr = (zval *) emalloc(sizeof(zval));
							**var_ptr = *orig_var;
							zendi_zval_copy_ctor(**var_ptr);
							(*var_ptr)->refcount=1;
						}
					}
					incdec_op(*var_ptr);
					switch (opline->opcode) {
						case ZEND_PRE_INC:
						case ZEND_PRE_DEC:
							Ts[opline->result.u.var].var = var_ptr;
							INC_AI_COUNT(&opline->result);
							break;
					}
				}
				break;
			case ZEND_PRINT_OP:
				zend_print_variable(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R));
				Ts[opline->result.u.var].tmp_var.value.lval = 1;
				Ts[opline->result.u.var].tmp_var.type = IS_LONG;
				FREE_OP(&opline->op1, free_op1);
				break;
			case ZEND_ECHO_OP:
				zend_print_variable(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R));
				FREE_OP(&opline->op1, free_op1);
				break;
			case ZEND_FETCH_R:
				zend_fetch_var_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_R ELS_CC);
				break;
			case ZEND_FETCH_W:
				zend_fetch_var_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_W ELS_CC);
				break;
			case ZEND_FETCH_RW:
				zend_fetch_var_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_RW ELS_CC);
				break;
			case ZEND_FETCH_IS:
				zend_fetch_var_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_IS ELS_CC);
				break;
			case ZEND_FETCH_DIM_R:
				zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_R ELS_CC);
				break;
			case ZEND_FETCH_DIM_W:
				zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_W ELS_CC);
				break;
			case ZEND_FETCH_DIM_RW:
				zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_RW ELS_CC);
				break;
			case ZEND_FETCH_DIM_IS:
				zend_fetch_dimension_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_IS ELS_CC);
				break;
			case ZEND_FETCH_OBJ_R:
				zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_R ELS_CC);
				break;
			case ZEND_FETCH_OBJ_W:
				zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_W ELS_CC);
				break;
			case ZEND_FETCH_OBJ_RW:
				zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_RW ELS_CC);
				break;
			case ZEND_FETCH_OBJ_IS:
				zend_fetch_property_address(&opline->result, &opline->op1, &opline->op2, Ts, BP_VAR_IS ELS_CC);
				break;
			case ZEND_FETCH_DIM_TMP_VAR:
				zend_fetch_dimension_address_from_tmp_var(&opline->result, &opline->op1, &opline->op2, Ts ELS_CC);
				break;
			case ZEND_ASSIGN: {
					zval *value = get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);

					zend_assign_to_variable(&opline->result, &opline->op1, value, (free_op2?IS_TMP_VAR:opline->op2.op_type), Ts ELS_CC);
					/* zend_assign_to_variable() always takes care of op2, never free it! */
				}
				break;
			case ZEND_ASSIGN_REF:
				zend_assign_to_variable_reference(&opline->result, get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_W), get_zval_ptr_ptr(&opline->op2, Ts, BP_VAR_W), Ts ELS_CC);
				INC_AI_COUNT(&opline->result);
				break;
			case ZEND_JMP:
#if DEBUG_ZEND>=2
				printf("Jumping to %d\n", opline->op1.u.opline_num);
#endif
				opline = &op_array->opcodes[opline->op1.u.opline_num];
				continue;
				break;
			case ZEND_JMPZ: {
					znode *op1 = &opline->op1;
					
					if (!i_zend_is_true(get_zval_ptr(op1, Ts, &free_op1, BP_VAR_R))) {
#if DEBUG_ZEND>=2
						printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op2.u.opline_num];
						FREE_OP(op1, free_op1);
						continue;
					}
					FREE_OP(op1, free_op1);
				}
				break;
			case ZEND_JMPNZ: {
					znode *op1 = &opline->op1;
					
					if (zend_is_true(get_zval_ptr(op1, Ts, &free_op1, BP_VAR_R))) {
#if DEBUG_ZEND>=2
						printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op2.u.opline_num];
						FREE_OP(op1, free_op1);
						continue;
					}
					FREE_OP(op1, free_op1);
				}
				break;
			case ZEND_JMPZNZ: {
					znode *res = &opline->result;
					
					if (!zend_is_true(get_zval_ptr(res, Ts, &free_op1, BP_VAR_R))) {
#if DEBUG_ZEND>=2
						printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op2.u.opline_num];
					} else {
#if DEBUG_ZEND>=2
						printf("Conditional jmp on true to %d\n", opline->op1.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op1.u.opline_num];
					}
					FREE_OP(res, free_op1);
				}
				continue;
				break;
			case ZEND_JMPZ_EX: {
					zend_op *original_opline = opline;
					int retval = zend_is_true(get_zval_ptr(&original_opline->op1, Ts, &free_op1, BP_VAR_R));
					
					if (!retval) {
#if DEBUG_ZEND>=2
						printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op2.u.opline_num];
						FREE_OP(&original_opline->op1, free_op1);
						Ts[original_opline->result.u.var].tmp_var.value.lval = retval;
						Ts[original_opline->result.u.var].tmp_var.type = IS_LONG;
						continue;
					}
					FREE_OP(&original_opline->op1, free_op1);
					Ts[original_opline->result.u.var].tmp_var.value.lval = retval;
					Ts[original_opline->result.u.var].tmp_var.type = IS_LONG;
				}
				break;
			case ZEND_JMPNZ_EX: {
					zend_op *original_opline = opline;
					int retval = zend_is_true(get_zval_ptr(&original_opline->op1, Ts, &free_op1, BP_VAR_R));
					
					if (retval) {
#if DEBUG_ZEND>=2
						printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
						opline = &op_array->opcodes[opline->op2.u.opline_num];
						FREE_OP(&original_opline->op1, free_op1);
						Ts[original_opline->result.u.var].tmp_var.value.lval = retval;
						Ts[original_opline->result.u.var].tmp_var.type = IS_LONG;
						continue;
					}
					FREE_OP(&original_opline->op1, free_op1);
					Ts[original_opline->result.u.var].tmp_var.value.lval = retval;
					Ts[original_opline->result.u.var].tmp_var.type = IS_LONG;
				}
				break;
			case ZEND_FREE:
				zendi_zval_dtor(Ts[opline->op1.u.var].tmp_var);
				break;
			case ZEND_INIT_STRING:
				Ts[opline->result.u.var].tmp_var.value.str.val = emalloc(1);
				Ts[opline->result.u.var].tmp_var.value.str.val[0] = 0;
				Ts[opline->result.u.var].tmp_var.value.str.len = 0;
				Ts[opline->result.u.var].tmp_var.refcount = 1;
				break;
			case ZEND_ADD_CHAR:
				add_char_to_string(	&Ts[opline->result.u.var].tmp_var,
									get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_NA),
									&opline->op2.u.constant);
				/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
				break;
			case ZEND_ADD_STRING:
				add_string_to_string(	&Ts[opline->result.u.var].tmp_var,
										get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_NA),
										&opline->op2.u.constant);
				/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
				break;
			case ZEND_ADD_VAR: {
					zval *var = get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);
					zval var_copy;
					int destroy=0;

					if (var->type != IS_STRING) {
						var_copy = *var;
						zval_copy_ctor(&var_copy);
						var = &var_copy;
						convert_to_string(var);
						destroy=1;
					}
					add_string_to_string(	&Ts[opline->result.u.var].tmp_var,
											get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_NA),
											var);
					if (destroy) {
						zval_dtor(var);
					}
					/* original comment, possibly problematic:
					 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
					 * (Zeev):  I don't think it's problematic, we only use variables
					 * which aren't affected by FREE_OP()'s anyway, unless they're
					 * string offsets or overloaded objects
					 */
					FREE_OP(&opline->op2, free_op2);
				}
				break;
			case ZEND_INIT_FCALL_BY_NAME: {
					zval *function_name = get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);
					zend_function *function;
					HashTable *active_function_table;
					zval tmp;

					tmp = *function_name;
					zval_copy_ctor(&tmp);
					convert_to_string(&tmp);
					function_name = &tmp;
					zend_str_tolower(tmp.value.str.val, tmp.value.str.len);
						
					if (opline->op1.op_type != IS_UNUSED) {
						if (opline->op1.op_type==IS_CONST) { /* used for class_name::function() */
							zend_class_entry *ce;

							if (zend_hash_find(EG(class_table), opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len+1, (void **) &ce)==FAILURE) {
								zend_error(E_ERROR, "Undefined class name '%s'", opline->op1.u.constant.value.str.val);
							}
							active_function_table = &ce->function_table;
							if (zend_hash_find(EG(active_symbol_table), "this", sizeof("this"), (void **) &object_ptr)==FAILURE) {
								object_ptr=NULL;
							}
						} else { /* used for member function calls */
							object_ptr = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_R);

							if (opline->op1.op_type==IS_VAR
								&& opline>EG(active_op_array)->opcodes
								&& (opline-1)->opcode == ZEND_JMP_NO_CTOR) {
								/* constructor call */
								EG(AiCount)++;
							}

							if (!object_ptr || (*object_ptr)->value.obj.ce->handle_function_call) { /* overloaded function call */
								zend_overloaded_element overloaded_element;
								zend_property_reference *property_reference;

								overloaded_element.element = *function_name;
								overloaded_element.type = IS_METHOD;

								if (object_ptr) {
									zend_property_reference property_reference;

									if ((*object_ptr)->type != IS_OBJECT) {
										zend_error(E_ERROR, "Call to a member function on a non-object");
									}

									property_reference.object = object_ptr;
									property_reference.type = BP_VAR_NA;
									zend_llist_init(&property_reference.elements_list, sizeof(zend_overloaded_element), NULL, 0);
									zend_stack_push(&EG(overloaded_objects_stack), &property_reference, sizeof(zend_property_reference));
								}
								zend_stack_top(&EG(overloaded_objects_stack), (void **) &property_reference);
								zend_llist_add_element(&property_reference->elements_list, &overloaded_element);
								function_being_called = (zend_function *) emalloc(sizeof(zend_function));
								function_being_called->type = ZEND_OVERLOADED_FUNCTION;
								function_being_called->common.arg_types = NULL;
								goto overloaded_function_call_cont;
							}

							if ((*object_ptr)->type != IS_OBJECT) {
								zend_error(E_ERROR, "Call to a member function on a non-object");
							}
							active_function_table = &(*object_ptr)->value.obj.ce->function_table;
						}
					} else { /* function pointer */
						object_ptr = NULL;
						active_function_table = EG(function_table);
					}
					if (zend_hash_find(active_function_table, function_name->value.str.val, function_name->value.str.len+1, (void **) &function)==FAILURE) {
						zend_error(E_ERROR, "Call to undefined function:  %s()\n", function_name->value.str.val);
					}
					zval_dtor(&tmp);
					function_being_called = function;
overloaded_function_call_cont:
					zend_ptr_stack_push(&EG(arg_types_stack), function_being_called); 
					FREE_OP(&opline->op2, free_op2);
				}
				goto init_by_name_cont;
				break;
			case ZEND_INIT_FCALL:
				object_ptr=NULL;
				function_being_called=NULL;
init_by_name_cont:
				zend_ptr_stack_push(&EG(function_symbol_table_stack), function_state.function_symbol_table);
				if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
					//printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);
					function_state.function_symbol_table = *(EG(symtable_cache_ptr)--);
				} else {
					function_state.function_symbol_table = (HashTable *) emalloc(sizeof(HashTable));
					zend_hash_init(function_state.function_symbol_table, 0, NULL, PVAL_PTR_DTOR, 0);
					//printf("Cache miss!  Initialized %x\n", function_state.function_symbol_table);
				}
				if (object_ptr && function_being_called && function_being_called->type!=ZEND_OVERLOADED_FUNCTION) {
					zval *dummy = (zval *) emalloc(sizeof(zval)), **this_ptr;

					var_uninit(dummy);
					dummy->refcount=1;
					dummy->is_ref=0;
					zend_hash_update_ptr(function_state.function_symbol_table, "this", sizeof("this"), dummy, sizeof(zval *), (void **) &this_ptr);
					zend_assign_to_variable_reference(NULL, this_ptr, object_ptr, NULL ELS_CC);
					object_ptr = NULL;
				}
				break;
			case ZEND_DO_FCALL: 
				if (function_being_called) {
					function_state.function = function_being_called;
				} else {
					zval *fname = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					
					if (zend_hash_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, (void **) &function_state.function)==FAILURE) {
						zend_error(E_ERROR, "Unknown function:  %s()\n", fname->value.str.val);
					}
					FREE_OP(&opline->op1, free_op1);
				}
				{
					zval *original_return_value;
					
					if (function_being_called) {
						function_being_called = zend_ptr_stack_pop(&EG(arg_types_stack));
					}
					calling_symbol_table = EG(active_symbol_table);
					EG(active_symbol_table) = function_state.function_symbol_table;
					if (function_state.function->type==ZEND_INTERNAL_FUNCTION) {
						var_uninit(&Ts[opline->result.u.var].tmp_var);
						((zend_internal_function *) function_state.function)->handler(EG(active_symbol_table), &Ts[opline->result.u.var].tmp_var, &EG(regular_list), &EG(persistent_list));
					} else if (function_state.function->type==ZEND_USER_FUNCTION) {
						original_return_value = EG(return_value);
						EG(return_value) = &Ts[opline->result.u.var].tmp_var;
						var_uninit(EG(return_value));
						EG(active_op_array) = (zend_op_array *) function_state.function;
						zend_execute(EG(active_op_array) ELS_CC);
						EG(opline_ptr) = &opline;
						EG(active_op_array) = op_array;
						EG(return_value)=original_return_value;
					} else { /* ZEND_OVERLOADED_FUNCTION */
						call_overloaded_function(EG(active_symbol_table), &Ts[opline->result.u.var].tmp_var, &EG(regular_list), &EG(persistent_list));
						efree(function_being_called);
					}
					function_state.function = (zend_function *) op_array;
					EG(active_symbol_table) = calling_symbol_table;
					if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
						zend_hash_destroy(function_state.function_symbol_table);
						efree(function_state.function_symbol_table);
					} else {
						*(++EG(symtable_cache_ptr)) = function_state.function_symbol_table;
						zend_hash_clean(*EG(symtable_cache_ptr));
					}
					EG(function_state_ptr) = &function_state;
					function_state.function_symbol_table = zend_ptr_stack_pop(&EG(function_symbol_table_stack));
				}
				break;
			case ZEND_RETURN: {
					zval *retval = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					
					*EG(return_value) = *retval;
					if (!free_op1) {
						zendi_zval_copy_ctor(*EG(return_value));
					}
					//return_value->refcount=1;
					//return_value->is_ref=0;
#if SUPPORT_INTERACTIVE
					op_array->last_executed_op_number = opline-op_array->opcodes;
#endif
					free_alloca(Ts);
					return;
				}
				break;
			case ZEND_SEND_VAL: 
				if (function_being_called
					&& function_being_called->common.arg_types
					&& opline->op2.u.opline_num<function_being_called->common.arg_types[0]
					&& function_being_called->common.arg_types[1+opline->op2.u.opline_num]==BYREF_FORCE) {
						zend_error(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
				}
				{
					zval *valptr = (zval *) emalloc(sizeof(zval));

					*valptr = Ts[opline->op1.u.var].tmp_var;
					valptr->refcount=1;
					valptr->is_ref=0;
					zend_hash_next_index_insert_ptr(function_state.function_symbol_table, valptr, sizeof(zval *), NULL);
				}
				break;
			case ZEND_SEND_VAR:
				if (function_being_called
					&& function_being_called->common.arg_types
					&& opline->op2.u.opline_num<function_being_called->common.arg_types[0]
					&& function_being_called->common.arg_types[1+opline->op2.u.opline_num]==BYREF_FORCE) {
						goto send_by_ref;
				}
				{
					zval *varptr = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);

					if (varptr == &EG(uninitialized_zval)) {
						varptr = (zval *) emalloc(sizeof(zval));
						var_uninit(varptr);
						varptr->refcount=0;
						varptr->is_ref=0;
					} else if (varptr->is_ref) {
						zval *original_var = varptr;

						varptr = (zval *) emalloc(sizeof(zval));
						*varptr = *original_var;
						varptr->is_ref = 0;
						varptr->refcount = 0;
						zval_copy_ctor(varptr);
					}
					varptr->refcount++;
					zend_hash_next_index_insert_ptr(function_state.function_symbol_table, varptr, sizeof(zval *), NULL);
				}
				break;
send_by_ref:
			case ZEND_SEND_REF: {
					zval **varptr_ptr = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_W);
					zval *varptr = *varptr_ptr;

					if (!varptr->is_ref) {
						/* code to break away this variable */
						if (varptr->refcount>1) {
							*varptr_ptr = (zval *) emalloc(sizeof(zval));
							**varptr_ptr = *varptr;
							varptr = *varptr_ptr;
							varptr->refcount = 1;
						}
						varptr->is_ref = 1;
						/* at the end of this code refcount is always 1 */
					}
					varptr->refcount++;
					zend_hash_next_index_insert_ptr(function_state.function_symbol_table, varptr, sizeof(zval *), NULL);
				}
				break;
			case ZEND_RECV: {
					zval **param;

					if (zend_hash_index_find(EG(active_symbol_table), opline->op1.u.constant.value.lval, (void **) &param)==FAILURE) {
						zend_error(E_NOTICE, "Missing argument %d for %s()\n", opline->op1.u.constant.value.lval, get_active_function_name());
					} else if ((*param)->is_ref) {
						zend_assign_to_variable_reference(NULL, get_zval_ptr_ptr(&opline->result, Ts, BP_VAR_W), param, NULL ELS_CC);
					} else {
						zend_assign_to_variable(NULL, &opline->result, *param, IS_VAR, Ts ELS_CC);
					}
				}
				break;
			case ZEND_RECV_INIT: {
					zval **param, *assignment_value;

					if (zend_hash_index_find(EG(active_symbol_table), opline->op1.u.constant.value.lval, (void **) &param)==FAILURE) {
						if (opline->op2.u.constant.type == IS_CONSTANT) {
							zval *default_value = (zval *) emalloc(sizeof(zval));
							zval tmp;

							*default_value = opline->op2.u.constant;
							if (!zend_get_constant(default_value->value.str.val, default_value->value.str.len, &tmp)) {
								default_value->type = IS_STRING;
								zval_copy_ctor(default_value);
							} else {
								*default_value = tmp;
							}
							default_value->refcount=0;
							default_value->is_ref=0;
							param = &default_value;
							assignment_value = default_value;
						} else {
							param = NULL;
							assignment_value = &opline->op2.u.constant;
						}
					} else {
						assignment_value = *param;
					}

					if (assignment_value->is_ref) {
						zend_assign_to_variable_reference(NULL, get_zval_ptr_ptr(&opline->result, Ts, BP_VAR_W), param, NULL ELS_CC);
					} else {
						zend_assign_to_variable(NULL, &opline->result, assignment_value, IS_VAR, Ts ELS_CC);
					}
				}
				break;
			case ZEND_BOOL:
				/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
				Ts[opline->result.u.var].tmp_var.value.lval = zend_is_true(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R));
				Ts[opline->result.u.var].tmp_var.type = IS_LONG;
				FREE_OP(&opline->op1, free_op1);
				break;
			case ZEND_BRK:
			case ZEND_CONT: {
					zval *nest_levels_zval = get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);
					zval tmp;
					int array_offset, nest_levels, original_nest_levels;
					zend_brk_cont_element *jmp_to;

					if (nest_levels_zval->type != IS_LONG) {
						tmp = *nest_levels_zval;
						zval_copy_ctor(&tmp);
						convert_to_long(&tmp);
						nest_levels = tmp.value.lval;
					} else {
						nest_levels = nest_levels_zval->value.lval;
					}
					original_nest_levels = nest_levels;
					array_offset = opline->op1.u.opline_num;
					do {
						if (array_offset==-1) {
							zend_error(E_ERROR, "Cannot break/continue %d levels\n", original_nest_levels);
						}
						jmp_to = &op_array->brk_cont_array[array_offset];
						array_offset = jmp_to->parent;
					} while (--nest_levels > 0);

					if (opline->opcode == ZEND_BRK) {
						opline = op_array->opcodes+jmp_to->brk;
					//	printf("Jumping to %d\n", jmp_to->brk);
					} else {
						opline = op_array->opcodes+jmp_to->cont;
					//	printf("Jumping to %d\n", jmp_to->cont);
					}
					FREE_OP(&opline->op2, free_op2);
					continue;
				}
				break;
			case ZEND_CASE:
				is_equal_function(&Ts[opline->result.u.var].tmp_var, 
							 get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R),
							 get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R) );
				FREE_OP(&opline->op2, free_op2);
				break;
			case ZEND_NEW: {
					zval *tmp = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					zval class_name;
					zend_class_entry *ce;

					class_name = *tmp;
					zval_copy_ctor(&class_name);
					convert_to_string(&class_name);
					zend_str_tolower(class_name.value.str.val, class_name.value.str.len);

					if (zend_hash_find(EG(class_table), class_name.value.str.val, class_name.value.str.len+1, (void **) &ce)==FAILURE) {
						zend_error(E_ERROR, "Cannot instanciate non-existant class:  %s", class_name.value.str.val);
					}
					object_init_ex(&Ts[opline->result.u.var].tmp_var, ce);

					zval_dtor(&class_name);
					FREE_OP(&opline->op1, free_op1);
				}
				break;
			case ZEND_FETCH_CONSTANT:
				if (!zend_get_constant(opline->op1.u.constant.value.str.val, opline->op1.u.constant.value.str.len, &Ts[opline->result.u.var].tmp_var)) {
					zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
								opline->op1.u.constant.value.str.val,
								opline->op1.u.constant.value.str.val);
					Ts[opline->result.u.var].tmp_var = opline->op1.u.constant;
					zval_copy_ctor(&Ts[opline->result.u.var].tmp_var);
				}
				break;
			case ZEND_INIT_ARRAY:
			case ZEND_ADD_ARRAY_ELEMENT: {
					zval *array_ptr = &Ts[opline->result.u.var].tmp_var;
					zval *expr=get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					zval *offset=get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);

					if (opline->opcode==ZEND_INIT_ARRAY) {
						array_init(array_ptr);
						if (!expr) {
							break;
						}
					}
					if (free_op1) { /* temporary variable */
						zval *new_expr = (zval *) emalloc(sizeof(zval));

						*new_expr = *expr;
						expr = new_expr;
						expr->refcount=1;
						expr->is_ref=0;
					} else {
						if (expr->is_ref) {
							zval *new_expr = (zval *) emalloc(sizeof(zval));

							*new_expr = *expr;
							expr = new_expr;
							zendi_zval_copy_ctor(*expr);
							expr->refcount=1;
							expr->is_ref=0;
						} else {
							expr->refcount++;
						}
					}
					if (offset) {
						switch(offset->type) {
							case IS_DOUBLE:
								zend_hash_index_update(array_ptr->value.ht, (long) offset->value.lval, &expr, sizeof(zval *), NULL);
								break;
							case IS_LONG:
								zend_hash_index_update(array_ptr->value.ht, offset->value.lval, &expr, sizeof(zval *), NULL);
								break;
							case IS_STRING:
								zend_hash_update(array_ptr->value.ht, offset->value.str.val, offset->value.str.len+1, &expr, sizeof(zval *), NULL);
								break;
							default:
								/* do nothing */
								break;
						}
						FREE_OP(&opline->op2, free_op2);
					} else {
						zend_hash_next_index_insert(array_ptr->value.ht, &expr, sizeof(zval *), NULL);
					}
				}
				break;
			case ZEND_CAST: {
					zval *expr = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					zval *result = &Ts[opline->result.u.var].tmp_var;

					*result = *expr;
					if (!free_op1) {
						zendi_zval_copy_ctor(*result);
					}					
					switch (opline->op2.u.constant.type) {
						case IS_LONG:
							convert_to_long(result);
							break;
						case IS_DOUBLE:
							convert_to_double(result);
							break;
						case IS_STRING:
							convert_to_string(result);
							break;
						case IS_ARRAY:
							convert_to_array(result);
							break;
						case IS_OBJECT:
							convert_to_object(result);
							break;
					}
				}
				break;
			case ZEND_INCLUDE_OR_EVAL: {
					zend_op_array *new_op_array=NULL;
					zval *original_return_value = EG(return_value);
					CLS_FETCH();

					switch (opline->op2.u.constant.value.lval) {
						case ZEND_INCLUDE:
							new_op_array = compile_filename(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R) CLS_CC);
							break;
						case ZEND_EVAL:
							new_op_array = compile_string(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R) CLS_CC);
							break;
					}
					if (new_op_array) {
						Ts[opline->result.u.var].tmp_var.value.lval = 1;
						Ts[opline->result.u.var].tmp_var.type = IS_LONG;
						EG(return_value) = &Ts[opline->result.u.var].tmp_var;
						EG(active_op_array) = new_op_array;
						zend_execute(new_op_array ELS_CC);

						EG(opline_ptr) = &opline;
						EG(active_op_array) = op_array;
						EG(function_state_ptr) = &function_state;
						destroy_op_array(new_op_array);
						efree(new_op_array);
					} else {
						var_uninit(&Ts[opline->result.u.var].tmp_var);
					}
					EG(return_value) = original_return_value;
					FREE_OP(&opline->op1, free_op1);
				}
				break;
			case ZEND_UNSET_VAR: {
					zval tmp, *variable = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);

					if (variable->type != IS_STRING) {
						tmp = *variable;
						zval_copy_ctor(&tmp);
						convert_to_string(&tmp);
						variable = &tmp;
					}

					zend_hash_del(EG(active_symbol_table), variable->value.str.val, variable->value.str.len+1);

					if (variable == &tmp) {
						zval_dtor(&tmp);
					}
					FREE_OP(&opline->op1, free_op1);
				}
				break;
			case ZEND_UNSET_DIM_OBJ: {
					zval **container = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_R);
					zval *offset = get_zval_ptr(&opline->op2, Ts, &free_op2, BP_VAR_R);

					if (container) {
						HashTable *ht;

						switch ((*container)->type) {
							case IS_ARRAY:
								ht = (*container)->value.ht;
								break;
							case IS_OBJECT:
								ht = (*container)->value.obj.properties;
								break;
							default:
								ht = NULL;
								break;
						}
						if (ht) {
							switch (offset->type) {
								case IS_LONG:
									zend_hash_index_del(ht, offset->value.lval);
									break;
								case IS_STRING:
									zend_hash_del(ht, offset->value.str.val, offset->value.str.len+1);
									break;
							}
						}
					} else {
						/* overloaded element */
					}
					FREE_OP(&opline->op2, free_op2);
				}
				break;
			case ZEND_FE_RESET: {
					zval *array = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);

					Ts[opline->result.u.var].tmp_var = *array;
					array = &Ts[opline->result.u.var].tmp_var;
					if (!free_op1) {
						zval_copy_ctor(array);
					}
					if (array->type == IS_ARRAY) {
						/* probably redundant */
						zend_hash_internal_pointer_reset(array->value.ht);
					} else {
						/* JMP to the end of foreach - TBD */
					}
				}
				break;
			case ZEND_FE_FETCH: {
					zval *array = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);
					zval *result = &Ts[opline->result.u.var].tmp_var;
					zval **value, *key;
					char *str_key;
					ulong int_key;

					if (zend_hash_get_current_data(array->value.ht, (void **) &value)==FAILURE) {
						opline = op_array->opcodes+opline->op2.u.opline_num;
						continue;
					}
					array_init(result);


					(*value)->refcount++;
					zend_hash_index_update(result->value.ht, 0, value, sizeof(zval *), NULL);

					key = (zval *) emalloc(sizeof(zval));
					key->refcount=1;
					key->is_ref=0;
					switch (zend_hash_get_current_key(array->value.ht, &str_key, &int_key)) {
						case HASH_KEY_IS_STRING:
							key->value.str.val = str_key;
							key->value.str.len = strlen(str_key);
							key->type = IS_STRING;
							break;
						case HASH_KEY_IS_LONG:
							key->value.lval = int_key;
							key->type = IS_LONG;
							break;
					}
					zend_hash_index_update(result->value.ht, 1, &key, sizeof(zval *), NULL);
					zend_hash_move_forward(array->value.ht);
				}
				break;
			case ZEND_JMP_NO_CTOR: {
					zval *object = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);

					EG(AiCount)++;
					if (!object->value.obj.ce->handle_function_call
						&& !zend_hash_exists(&object->value.obj.ce->function_table, object->value.obj.ce->name, object->value.obj.ce->name_length+1)) {
						opline = op_array->opcodes + opline->op2.u.opline_num;
						continue;
					}
				}
				break;
			case ZEND_ISSET_ISEMPTY: {
					zval **var = get_zval_ptr_ptr(&opline->op1, Ts, BP_VAR_IS);
					int isset;

					if (var==&EG(uninitialized_zval_ptr)
						|| ((*var)->type == IS_STRING && (*var)->value.str.val == undefined_variable_string)) {
						isset = 0;
					} else {
						isset = 1;
					}

					switch (opline->op2.u.constant.value.lval) {
						case ZEND_ISSET:
							Ts[opline->result.u.var].tmp_var.value.lval = isset;
							break;
						case ZEND_ISEMPTY:
							if (!isset || !zend_is_true(*var)) {
								Ts[opline->result.u.var].tmp_var.value.lval = 1;
							} else {
								Ts[opline->result.u.var].tmp_var.value.lval = 0;
							}
							break;
					}
					Ts[opline->result.u.var].tmp_var.type = IS_BOOL;
				}
				break;
			case ZEND_EXIT:
				if (opline->op1.op_type != IS_UNUSED) {
					zend_print_variable(get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R));
					FREE_OP(&opline->op1, free_op1);
				}
				zend_bailout();
				break;
			case ZEND_BEGIN_SILENCE:
				Ts[opline->result.u.var].tmp_var.value.lval = EG(error_reporting);
				Ts[opline->result.u.var].tmp_var.type = IS_LONG;  /* shouldn't be necessary */
				EG(error_reporting) = 0;
				break;
			case ZEND_END_SILENCE:
				EG(error_reporting) = Ts[opline->op1.u.var].tmp_var.value.lval;
				break;
			case ZEND_QM_ASSIGN: {
					zval *value = get_zval_ptr(&opline->op1, Ts, &free_op1, BP_VAR_R);

					Ts[opline->result.u.var].tmp_var = *value;
					if (!free_op1) {
						zval_copy_ctor(&Ts[opline->result.u.var].tmp_var);
					}
				}
				break;
			case ZEND_EXT_STMT: 
				if (!EG(no_extensions)) {
					zend_llist_apply_with_argument(&zend_extensions, (void (*)(void *, void *)) zend_extension_statement_handler, op_array);
				}
				break;
			case ZEND_EXT_FCALL_BEGIN:
				if (!EG(no_extensions)) {
					zend_llist_apply_with_argument(&zend_extensions, (void (*)(void *, void *)) zend_extension_fcall_begin_handler, op_array);
				}
				break;
			case ZEND_EXT_FCALL_END:
				if (!EG(no_extensions)) {
					zend_llist_apply_with_argument(&zend_extensions, (void (*)(void *, void *)) zend_extension_fcall_end_handler, op_array);
				}
				break;
			case ZEND_EXT_NOP:
			case ZEND_NOP:
				break;
			default:
				break;
		}
		opline++;
	}
#if SUPPORT_INTERACTIVE
	op_array->last_executed_op_number = opline-op_array->opcodes;
#endif
	free_alloca(Ts);
}
