--TEST--
Bug #74004 (DOMDocument->loadHTML and ->loadHTMLFile do not heed LIBXML_NOWARNING and LIBXML_NOERROR options)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc=new DOMDocument();
libxml_use_internal_errors(true);
$doc->loadHTML("<tag-throw-warning></tag-throw-warning>",LIBXML_NOWARNING|LIBXML_NOERROR);
print count(libxml_get_errors());

?>
--EXPECT--
0
