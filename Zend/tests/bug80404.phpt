--TEST--
Bug #80404: Incorrect range inference result when division results in float
--FILE--
<?php

$n = 63;
var_dump((int) ($n / 120 * 100));

?>
--EXPECT--
int(52)
