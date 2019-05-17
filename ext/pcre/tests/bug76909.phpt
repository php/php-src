--TEST--
Bug #76909 preg_match difference between 7.3 and < 7.3
--SKIPIF--
<?php
if(!ini_get("pcre.jit")) {
	echo "skip JIT is disabled";
}
?>
--FILE--
<?php

$data = " domain.com";
$reg0 = "/^[\x{0100}-\x{017f}]{1,63}$/iu";
$reg1 = "/(*NO_JIT)^[\x{0100}-\x{017f}]{1,63}$/iu";

$n0 = preg_match($reg0, $data, $m0);
$n1 = preg_match($reg1, $data, $m1);

var_dump($n0, $n1, count($m0), count($m1));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
