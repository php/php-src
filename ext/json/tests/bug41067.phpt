--TEST--
Bug #41067 (json_encode() problem with UTF-16 input)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$single_barline = "\360\235\204\200";
$array = array($single_barline);
var_dump(bin2hex((binary)$single_barline));

$json = json_encode($array);
$json_decoded = json_decode($json, true);

var_dump(bin2hex((binary)$json_decoded[0]));

?>
--EXPECT--
unicode(8) "f09d8480"
unicode(8) "f09d8480"
