--TEST--
Bug #52211 (iconv() returns part of string on error)
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php

$str = "PATHOLOGIES MÉDICO-CHIRUR. ADUL. PL";
$str_iconv = iconv('CP850', 'ISO-8859-1', $str );
var_dump($str_iconv);

?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
