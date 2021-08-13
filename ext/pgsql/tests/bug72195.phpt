--TEST--
Bug #72195 (pg_pconnect/pg_connect cause use-after-free)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
$val = [];
try {
    pg_pconnect($var1, "2", "3", "4");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $var1 in %s on line %d
pg_pconnect() expects at most 2 arguments, 4 given
