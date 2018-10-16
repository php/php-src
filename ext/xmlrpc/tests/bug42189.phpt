--TEST--
Bug #42189 (xmlrpc_get_type() crashes PHP on invalid dates)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php
$a = '~~~~~~~~~~~~~~~~~~';
$ok = xmlrpc_set_type($a, 'datetime');
var_dump($ok);

echo "Done\n";
?>
--EXPECT--
bool(false)
Done
