// -*- Mode: C++ -*-
//
// $Id$
//
// Created on: <09-Nov-2000 12:00:24 root>
//

#ifndef QDOM_QT_H
#define QDOM_QT_H

struct qdom_node
{
    int Type;
    char *Name;
    char *Content;
    void *Q_Node;
};

struct qdom_attribute
{
    int Count;
    void *Q_Node;
};

struct qdom_doc
{
    void *Document;
    void *CurrentNode;
    struct qdom_node *Children;
};

struct qdom_message
{
    char *Log;
    void *OldHandler;
};

void qdom_init();
void qdom_shutdown();

void qdom_do_install_message_handler();
void qdom_do_free_message_handler();

char *qdom_error_log();

void qdom_do_version( char **ver );

struct qdom_node *qdom_do_next_node( struct qdom_node *node );
struct qdom_node *qdom_do_first_child( struct qdom_node *node );

struct qdom_attribute *qdom_do_node_attributes( struct qdom_node *node );
struct qdom_node *qdom_do_attribute_at( struct qdom_attribute *attr, int index );
void qdom_do_attributes_free( struct qdom_attribute *node );

int qdom_do_node_children_count( struct qdom_node *node );
int qdom_do_node_attribute_count( struct qdom_node *node );

struct qdom_node *qdom_do_copy_node( struct qdom_node *node );
void qdom_do_node_free( struct qdom_node *node );

void qdom_do_doc_type( struct qdom_doc *doc, char **name );
struct qdom_doc *qdom_do_init( const char *arg );
void qdom_do_free( struct qdom_doc *doc );

struct qdom_message *g_qdom_message_log;

#endif // QDOM_QT_H
