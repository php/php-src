--TEST--
ibase_trans(): Basic operations
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

$trans = ibase_trans(IBASE_DEFAULT, $x);
$sth = ibase_prepare($trans, 'INSERT INTO test1 VALUES (?, ?)');

$res = ibase_execute($sth, 100, 100);
var_dump($res);

ibase_commit($trans);

$rs = ibase_query($x, 'SELECT * FROM test1 WHERE i = 100');
var_dump(ibase_fetch_assoc($rs));

ibase_free_query($sth);
unset($res);

?>
--EXPECT--
int(1)
array(2) {
  ["I"]=>
  int(100)
  ["C"]=>
  string(3) "100"
}
