--TEST--
GH-13119 (float, double value is incorrect)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");

$dbh->exec('CREATE TABLE gh13119 (f_val FLOAT, d_val DOUBLE PRECISION)');

$dbh->exec('INSERT INTO gh13119 VALUES (0.1, 0.1)');
$dbh->exec('INSERT INTO gh13119 VALUES (0.0000000000000001, 0.0000000000000001)');
$dbh->exec('INSERT INTO gh13119 VALUES (12.000000, 12.00000000000000)');
$dbh->exec('INSERT INTO gh13119 VALUES (12.000001, 12.00000000000001)');
$dbh->exec('INSERT INTO gh13119 VALUES (12.345678, 12.34567890123456)');
$dbh->exec('INSERT INTO gh13119 VALUES (0.0000000000000000012345678, 0.000000000000000001234567890123456)');

$stmt = $dbh->query('select * from gh13119');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE gh13119');
unset($dbh);
?>
--EXPECT--
array(6) {
  [0]=>
  array(2) {
    ["F_VAL"]=>
    string(3) "0.1"
    ["D_VAL"]=>
    string(3) "0.1"
  }
  [1]=>
  array(2) {
    ["F_VAL"]=>
    string(7) "1.0E-16"
    ["D_VAL"]=>
    string(7) "1.0E-16"
  }
  [2]=>
  array(2) {
    ["F_VAL"]=>
    string(2) "12"
    ["D_VAL"]=>
    string(2) "12"
  }
  [3]=>
  array(2) {
    ["F_VAL"]=>
    string(9) "12.000001"
    ["D_VAL"]=>
    string(17) "12.00000000000001"
  }
  [4]=>
  array(2) {
    ["F_VAL"]=>
    string(9) "12.345678"
    ["D_VAL"]=>
    string(17) "12.34567890123456"
  }
  [5]=>
  array(2) {
    ["F_VAL"]=>
    string(13) "1.2345678E-18"
    ["D_VAL"]=>
    string(21) "1.234567890123456E-18"
  }
}
