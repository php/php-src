--TEST--
Bug #20108 (Segfault on printf statement)
--FILE--
<?php
	$a = "boo";
	$z = sprintf("%580.58s\n", $a);
	var_dump($z);
?>
--EXPECT--
string(581) "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 boo
"
