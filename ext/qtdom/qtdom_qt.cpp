// 
// $Id$
//
// Jan Borsodi <jb@ez.no>
// Created on: <09-Nov-2000 11:18:46 root>
//


extern "C"
{
//#include "php.h"
//#include "php_ini.h"
//#include "php_qtdom.h"
#include "qtdom_qt.h"
}


//#if HAVE_QTDOM

#include <qdom.h>
#include <qstring.h>
#include <qglobal.h>

struct qdom_node *qdom_to_node( QDomNode *node );

/*!
  Initialises certain global variables, they are:
  g_qdom_message_log : A global variable for handling error logs and message handler function.
*/

extern "C" void qdom_init()
{
    g_qdom_message_log = new qdom_message;
    g_qdom_message_log->Log = 0;
    g_qdom_message_log->OldHandler = 0;
}

/*!
  Frees global variables initialised in the init function.
*/

extern "C" void qdom_shutdown()
{
    if ( g_qdom_message_log->Log )
        delete []g_qdom_message_log->Log;
    delete g_qdom_message_log;
    g_qdom_message_log = 0;
}

/*!
  Copies the version number for Qt into the \c ver variable,
  the variable must be allocated by user and must have enough characters (20 should suffice).
*/

extern "C" void qdom_do_version( char **ver )
{
    strcpy( *ver, QT_VERSION_STR );
}

/*!
  Moves the DOM node to the next sibling if any and returns the node.
*/

extern "C" struct qdom_node *qdom_do_next_node( struct qdom_node *node )
{
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;

    if ( q_node->isNull() )
        return 0;
    *q_node = q_node->nextSibling();
    if ( q_node->isNull() )
        return 0;

    node->Type = q_node->nodeType();
    const char *name = q_node->nodeName().latin1();
    node->Name = new char[q_node->nodeName().length()+1];
    strcpy( node->Name, name );
    const char *content = q_node->nodeValue().latin1();
    node->Content = new char[q_node->nodeValue().length()+1];
    strcpy( node->Content, content );

    return node;
}

/*!
  Finds the first child of the current node and returns it if any.
*/

extern "C" struct qdom_node *qdom_do_first_child( struct qdom_node *node )
{
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;

    if ( q_node->isNull() )
        return 0;
    *q_node = q_node->firstChild();
    if ( q_node->isNull() )
        return 0;

    node->Type = q_node->nodeType();
    const char *name = q_node->nodeName().latin1();
    node->Name = new char[q_node->nodeName().length()+1];
    strcpy( node->Name, name );
    const char *content = q_node->nodeValue().latin1();
    node->Content = new char[q_node->nodeValue().length()+1];
    strcpy( node->Content, content );

    return node;
}

/*!
  Returns the number of the children for the current node.
*/

extern "C" int qdom_do_node_children_count( struct qdom_node *node )
{
    if ( !node )
        return 0;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;
    return q_node->childNodes().count();
}

/*!
  Returns the number of attributes for the current node.
*/

extern "C" int qdom_do_node_attribute_count( struct qdom_node *node )
{
    if ( !node )
        return 0;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;
    return q_node->attributes().length();
}

/*!
  Returns the attribute map for the current node.
*/

extern "C" struct qdom_attribute *qdom_do_node_attributes( struct qdom_node *node )
{
    struct qdom_attribute *attr = new struct qdom_attribute;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    QDomNamedNodeMap *map = new QDomNamedNodeMap( q_node->attributes() );
    attr->Q_Node = map;
    attr->Count = map->length();
    return attr;
}

/*!
  Returns the node at a given index taken from the attribute list if any.
*/

extern "C" struct qdom_node *qdom_do_attribute_at( struct qdom_attribute *attr, int index )
{
    if ( !attr )
        return 0;
    QDomNamedNodeMap *map = (QDomNamedNodeMap *)attr->Q_Node;
    if ( index < 0 || index >= map->length() )
        return 0;
    QDomNode node = map->item( index );
    return qdom_to_node( &node );
}

/*!
  Frees an attribute map.
*/

extern "C" void qdom_do_attributes_free( struct qdom_attribute *attr )
{
    if ( !attr )
        return;
    QDomNamedNodeMap *map = (QDomNamedNodeMap *)attr->Q_Node;
    delete map;
    delete attr;
}

/*!
  Makes a copy of a node.
*/

extern "C" struct qdom_node *qdom_do_copy_node( struct qdom_node *node )
{
    if ( !node )
        return 0;
    struct qdom_node *tmp = new struct qdom_node;
    if ( node->Name )
    {
        tmp->Name = new char[strlen(node->Name)+1];
        strcpy( tmp->Name, node->Name );
    }
    else
        tmp->Name = 0;
    if ( node->Content )
    {
        tmp->Content = new char[strlen(node->Content)+1];
        strcpy( tmp->Content, node->Content );
    }
    else
        tmp->Content = 0;
    if ( node->Q_Node )
    {
        QDomNode *q_node = (QDomNode *)node->Q_Node;
        tmp->Q_Node = new QDomNode( *q_node );
    }
    else
        tmp->Q_Node = 0;
    tmp->Type = node->Type;
    return tmp;
}

/*!
  Frees a node.
*/

extern "C" void qdom_do_node_free( struct qdom_node *node )
{
    if ( !node )
        return;
    delete []node->Name;
    delete []node->Content;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    delete q_node;
    delete node;
}

/*!
  Wraps a qdom_node struct around a QDomNode object which can be used by the C code.
*/

struct qdom_node *qdom_to_node( QDomNode *node )
{
    if ( !node )
        return 0;
    qdom_node *q_node = new struct qdom_node;
    q_node->Type = node->nodeType();
    const char *name = node->nodeName().latin1();
    q_node->Name = new char[node->nodeName().length()+1];
    strcpy( q_node->Name, name );
    const char *content = node->nodeValue().latin1();
    q_node->Content = new char[node->nodeValue().length()+1];
    strcpy( q_node->Content, content );
    q_node->Q_Node = new QDomNode( *node );
    return q_node;
}

/*!
  Copies the doctype name taken from the the qdom_doc object to the
  \c name variable, the variable is initialised by the function.
*/

extern "C" void qdom_do_doc_type( struct qdom_doc *doc, char **name )
{
    if ( !doc )
    {
        *name = 0;
        return;
    }
    QDomDocument *document = (QDomDocument *)doc->Document;
    QString str = document->doctype().name();
    const char *q_name = str.latin1();
    if ( q_name )
    {
        *name = new char[strlen(q_name)+1];
        strcpy( *name, q_name );
    }
    else
    {
        *name = 0;
    }
}

/*!
  Initialises a qdom_doc struct with the string taken from \c arg.
*/

extern "C" struct qdom_doc *qdom_do_init( const char *arg )
{
    struct qdom_doc *doc = new struct qdom_doc;
    QDomDocument *document = new QDomDocument();
    document->setContent( QString( arg ) );
    QDomNode *node = new QDomNode;
    *node = document->documentElement();
    doc->Document = document;
    doc->CurrentNode = node;
    doc->Children = qdom_to_node( node );
    return doc;
}

/*!
  Frees a qdom_doc struct.
*/

extern "C" void qdom_do_free( struct qdom_doc *doc )
{
    QDomNode *node = (QDomNode *)doc->CurrentNode;
    QDomDocument *document = (QDomDocument *)doc->Document;
    delete document;
    delete node;
    delete doc->Children;
    delete doc;
}

/*!
  The custom message output used for catching Qt error messages when parsing with QDOM.
*/

void qdom_messageOutput( QtMsgType , const char *msg )
{
    if ( !g_qdom_message_log )
        return;
    int msg_len = strlen( msg );
    int log_len = 0;
    if ( g_qdom_message_log->Log )
        log_len = strlen( g_qdom_message_log->Log );
    int total_len = log_len+msg_len+2;
    char *log = new char[total_len];
    if ( g_qdom_message_log->Log )
        strncpy( log, g_qdom_message_log->Log, log_len );
    strncpy( log+log_len, msg, msg_len );
    log[log_len+msg_len] = '\n';
    log[total_len - 1] = '\0';
    if ( g_qdom_message_log->Log )
        delete []g_qdom_message_log->Log;
    g_qdom_message_log->Log = log;
}

/*!
  Installs the custom message handler and clears the log entries.
*/

extern "C" void qdom_do_install_message_handler()
{
    if ( !g_qdom_message_log )
        g_qdom_message_log = new qdom_message;
    msg_handler *old_handler = new msg_handler;
    g_qdom_message_log->OldHandler = (void *)old_handler;
    if ( g_qdom_message_log->Log )
        delete []g_qdom_message_log->Log;
    g_qdom_message_log->Log = 0;
    *old_handler = qInstallMsgHandler( qdom_messageOutput );
}

/*!
  Frees the custom message handler.
*/

extern "C" void qdom_do_free_message_handler()
{
    msg_handler *old_handler = (msg_handler *)g_qdom_message_log->OldHandler;
    qInstallMsgHandler( *old_handler );
}

/*!
  Returns the string containg the error log, or 0 if no log is available.
*/

extern "C" char *qdom_error_log()
{
    if ( !g_qdom_message_log )
        return 0;
    return g_qdom_message_log->Log;
}

//#endif // HAVE_QTDOM
