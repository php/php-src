--TEST--
Test xml_set_start_namespace_decl_handler function: basic
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: true);
?>
--FILE--
<?php
require __DIR__ . '/xml_set_start_namespace_decl_handler_basic.inc';
?>
--EXPECT--
bool(true)
bool(true)
Namespace_Start_Handler called
...Prefix: aw1
...Uri: http://www.somewhere.com/namespace1
Namespace_Start_Handler called
...Prefix: aw2
...Uri: file:/DTD/somewhere.dtd
Namespace_End_Handler called
...Prefix: aw2

Namespace_End_Handler called
...Prefix: aw1

Done
