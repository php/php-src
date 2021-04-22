--TEST--
simplexml_import_dom() - Basic test on invalid nodetype error
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) {
    exit('Skip - SimpleXML extension not loaded');
}
?>
--FILE--
<?php
$x = new stdClass();

var_dump(simplexml_import_dom($x));
?>
--EXPECTF--
Warning: simplexml_import_dom(): Invalid Nodetype to import in %s on line %d

Fatal error: Arginfo / zpp mismatch during call of simplexml_import_dom() in %s on line %d
