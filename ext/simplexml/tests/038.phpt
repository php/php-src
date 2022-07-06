--TEST--
SimpleXML: Property assignment return value
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<root></root>
EOF;
$root = simplexml_load_string($xml);
var_dump($root->prop = 42);
?>
--EXPECT--
int(42)
