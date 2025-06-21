--TEST--
num_available_processors() tests
--SKIPIF--
<?php
if (!function_exists("num_available_processors")) die("skip");
?>
--FILE--
<?php

var_dump(num_available_processors());

echo "Done\n";
?>
--EXPECTF--
int(%d)
Done
