--TEST--
Bug #72195 (pg_pconnect/pg_connect cause use-after-free)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
$val = [];
$var1 = $val;
printf("%x\n", count($val));
@pg_pconnect($var1, "2", "3", "4");
$var1 = "";
tempnam(sys_get_temp_dir(), 'ABCDEFGHI');
printf("%x\n", count($val));
?>
--EXPECT--
0
0
