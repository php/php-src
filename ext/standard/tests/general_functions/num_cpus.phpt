--TEST--
num_cpus() tests
--SKIPIF--
<?php
if (!function_exists("num_cpus")) die("skip");
?>
--FILE--
<?php

var_dump(num_cpus());

echo "Done\n";
?>
--EXPECTF--
int(%d)
Done
