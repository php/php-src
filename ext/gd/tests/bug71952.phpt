--TEST--
Bug #71952 (Corruption inside imageaffinematrixget)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$vals=[str_repeat("A","200"),0,1,2,3,4,5,6,7,8,9];
imageaffinematrixget(4,$vals[0]);
var_dump($vals[0]);
?>
--EXPECTF--
string(200) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"