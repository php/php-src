/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jan Borsodi                                                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "qtdom.h"
#include "qtdom_qt.h"

#if HAVE_QTDOM

static zend_function_entry qdomdoc_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry qdomnode_class_functions[] = {
	{NULL, NULL, NULL}
};

/* If you declare any globals in php_qtdom.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(qtdom)
*/

/* True global resources - no need for thread safety here */
static int le_qtdom;

/* Every user visible function must have an entry in qtdom_functions[].
*/
function_entry qtdom_functions[] = {
	PHP_FE(qdom_tree,	NULL)
  	PHP_FE(qdom_error,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in qtdom_functions[] */
};

zend_module_entry qtdom_module_entry = {
	STANDARD_MODULE_HEADER,
	"qtdom",
	qtdom_functions,
	PHP_MINIT(qtdom),
	PHP_MSHUTDOWN(qtdom),
	NULL,
	NULL,
	PHP_MINFO(qtdom),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_QTDOM
ZEND_GET_MODULE(qtdom)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(qtdom)
{
	zend_class_entry qdomdoc_class_entry;
	zend_class_entry qdomnode_class_entry;

	INIT_CLASS_ENTRY(qdomdoc_class_entry, "QDomDocument", qdomdoc_class_functions);
	INIT_CLASS_ENTRY(qdomnode_class_entry, "QDomNode", qdomnode_class_functions);

	qdomdoc_class_entry_ptr = zend_register_internal_class(&qdomdoc_class_entry TSRMLS_CC);
	qdomnode_class_entry_ptr = zend_register_internal_class(&qdomnode_class_entry TSRMLS_CC);

    qdom_init();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(qtdom)
{
    qdom_shutdown();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(qtdom)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "qtdom support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ qdom_find_attributes
 * Helper function for creating the attributes, returns the number 
 * of attributes found or -1 if an error occured */
static int qdom_find_attributes( zval **children, struct qdom_attribute *attr TSRMLS_DC)
{
    zval *child;
    struct qdom_node *node;
    int count;
    int i;
    count = 0;

    MAKE_STD_ZVAL(*children);
    if (array_init(*children) == FAILURE)
        return -1;
    for ( i = 0; i < attr->Count; ++i )
    {
        node = qdom_do_attribute_at( attr, i );

        MAKE_STD_ZVAL(child);
        object_init_ex(child, qdomnode_class_entry_ptr);
        zend_hash_next_index_insert(Z_ARRVAL_PP(children), &child, sizeof(zval *), NULL);
        add_property_stringl(child, "name", (char *) node->Name, strlen(node->Name), 1);
        add_property_long(child, "type", node->Type);
        add_property_stringl(child, "content", (char *) node->Content, strlen(node->Content), 1);

        qdom_do_node_free( node );
        ++count;
    }

    return count;
}
/* }}} */

/* {{{ qdom_find_children
   Helper function for recursively going trough the QDomNode tree and creating
   the same in PHP objects. Returns the number of children or -1 if an error
   occured. */
static int qdom_find_children( zval **children, struct qdom_node *orig_node TSRMLS_DC)
{
    zval *child;
    struct qdom_node *node, *tmp_node, *child_node;
    int count;

    zval *n_children, *a_children;
    count = 0;

    node = qdom_do_copy_node( orig_node );
    tmp_node = node;
/*      node = orig_node; */

    MAKE_STD_ZVAL(*children);
    if (array_init(*children) == FAILURE)
        return -1;
    while( node )
    {
        int num_childs, num_attrs;

        MAKE_STD_ZVAL(child);
        object_init_ex(child, qdomnode_class_entry_ptr);
        zend_hash_next_index_insert(Z_ARRVAL_PP(children), &child, sizeof(zval *), NULL);
        add_property_stringl(child, "name", (char *) node->Name, strlen(node->Name), 1);
        add_property_long(child, "type", node->Type);
        if ( node->Type == 2 || node->Type == 3 || node->Type == 4 )
            add_property_stringl(child, "content", (char *) node->Content, strlen(node->Content), 1);

        num_attrs = qdom_do_node_attribute_count( node );
        if ( num_attrs > 0 )
        {
            struct qdom_attribute *attr = qdom_do_node_attributes( node );
            if ( qdom_find_attributes( &a_children, attr TSRMLS_CC) > 0 )
            {
                zend_hash_update(Z_OBJPROP_P(child), "attributes", sizeof("attributes"),
								(void *) &a_children, sizeof(zval *), NULL);
            }
            qdom_do_attributes_free( attr );
/*              add_property_long(child, "attributes", num_attrs ); */
        }

        num_childs = qdom_do_node_children_count( node );
        if ( num_childs > 0 )
        {
            child_node = qdom_do_copy_node( node );
            child_node = qdom_do_first_child( child_node );
            if ( qdom_find_children( &n_children, child_node TSRMLS_CC) > 0 )
            {
                zend_hash_update(Z_OBJPROP_P(child), "children", sizeof("children"), (void *) &n_children, sizeof(zval *), NULL);
            }
            qdom_do_node_free( child_node );
        }

        node = qdom_do_next_node( node );
        ++count;
    }
    qdom_do_node_free( tmp_node );
    return count;
}
/* }}} */

/* {{{ proto object qdom_tree( string )
   creates a tree of an xml string */
PHP_FUNCTION(qdom_tree)
{
	zval *arg;
    char qt_ver1[200];
    char *qt_ver = qt_ver1;
    char *qdom_type_name;
    struct qdom_doc *doc;
    struct qdom_node *node;
    zval *children;

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &arg) == FAILURE)
    {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

    qdom_do_install_message_handler();

    qdom_do_version( &qt_ver );

    object_init_ex(return_value, qdomdoc_class_entry_ptr);
	add_property_stringl(return_value, "version", (char *) qt_ver, strlen(qt_ver), 1);

    doc = qdom_do_init( Z_STRVAL_P(arg) );

    qdom_do_doc_type( doc, &qdom_type_name );

    if ( qdom_type_name )
        add_property_stringl(return_value, "doctype", (char *) qdom_type_name, strlen(qdom_type_name), 1);

    node = doc->Children;
    if ( qdom_find_children( &children, node TSRMLS_CC) > 0 )
    {
        add_property_long(return_value, "type", node->Type);
        zend_hash_update(Z_OBJPROP_P(return_value), "children", sizeof("children"), (void *) &children, sizeof(zval *), NULL);
    }

    qdom_do_free( doc );
    qdom_do_free_message_handler();
}
/* }}} */

/* {{{ proto string qdom_error()
   Returns the error string from the last QDOM operation or FALSE if no errors occured.*/
PHP_FUNCTION(qdom_error)
{
    char *error = qdom_error_log();
    if ( error == 0 )
        RETURN_FALSE;
    RETURN_STRING( error, 1 );
}
/* }}} */

#endif	/* HAVE_QTDOM */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
