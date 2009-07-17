--TEST--
xmlwriter_write_dtd with missing param(s)
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
$xmlwriter = xmlwriter_open_memory();
var_dump(xmlwriter_write_dtd($xmlwriter));
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
--EXPECTF--

Warning: xmlwriter_write_dtd() expects at least %d parameters, %d given in %s on line %d
NULL
