--TEST--
Segfault on printf statement bug #20108
--SKIPIF--
--FILE--
<?php
	$a = "boo";
	$z = sprintf("%580.58s\n", $a);
	var_dump($z);
?>
--EXPECT--
string(581) "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 boo
"
