// 
// $Id$
//
// Implementation of ||| class
//
// Jan Borsodi <jb@ez.no>
// Created on: <09-Nov-2000 11:18:46 root>
//
// Copyright (C) 1999-2000 eZ Systems.  All rights reserved.
//


extern "C"
{
//#include "php.h"
//#include "php_ini.h"
//#include "php_qtdom.h"
#include "qtdom_qt.h"
}


//#if HAVE_QTDOM

//  #include "ezxmlparser.hpp"
#include <qdom.h>
#include <qstring.h>
#include <qglobal.h>

struct qdom_node *qdom_to_node( QDomNode *node );

extern "C" void qdom_do_version( char **ver )
{
    strcpy( *ver, QT_VERSION_STR );
}

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

extern "C" int qdom_do_node_children_count( struct qdom_node *node )
{
    if ( !node )
        return 0;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;
    return q_node->childNodes().count();
}

extern "C" int qdom_do_node_attribute_count( struct qdom_node *node )
{
    if ( !node )
        return 0;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    if ( !q_node )
        return 0;
    return q_node->attributes().length();
}

extern "C" struct qdom_attribute *qdom_do_node_attributes( struct qdom_node *node )
{
    struct qdom_attribute *attr = new struct qdom_attribute;
    QDomNode *q_node = (QDomNode *)node->Q_Node;
    QDomNamedNodeMap *map = new QDomNamedNodeMap( q_node->attributes() );
    attr->Q_Node = map;
    attr->Count = map->length();
    return attr;
}

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

extern "C" void qdom_do_attributes_free( struct qdom_attribute *attr )
{
    if ( !attr )
        return;
    QDomNamedNodeMap *map = (QDomNamedNodeMap *)attr->Q_Node;
    delete map;
    delete attr;
}

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

extern "C" void qdom_do_free( struct qdom_doc *doc )
{
    QDomNode *node = (QDomNode *)doc->CurrentNode;
    QDomDocument *document = (QDomDocument *)doc->Document;
    delete document;
    delete node;
    delete doc->Children;
    delete doc;
}

//#endif // HAVE_QTDOM
