--TEST--
SimpleXML: Split text content
--SKIPIF--
<?php 
	if (!extension_loaded('simplexml')) print 'skip';
?>
--FILE--
<?php 

$xml =b<<<EOF
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo>bar<baz/>bar</foo>
EOF;

$sxe = simplexml_load_string($xml);

var_dump((string)$sxe);

?>
===DONE===
--EXPECT--
unicode(6) "barbar"
===DONE===
