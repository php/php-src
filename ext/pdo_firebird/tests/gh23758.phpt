--TEST--
GH-23758 (PDO_Firebird returns null for non-null empty BLOBs)
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

$dbh = getDbConnection();
$dbh->exec('CREATE TABLE gh23758 (ID INTEGER, BIN_VAL BLOB SUB_TYPE BINARY, TEXT_VAL BLOB SUB_TYPE TEXT)');

$stmt = $dbh->prepare('INSERT INTO gh23758 VALUES (?, ?, ?)');
$stmt->execute([1, null, null]);
$stmt->execute([2, '', '']);

$stmt = $dbh->query('SELECT BIN_VAL, TEXT_VAL FROM gh23758 ORDER BY ID');
var_dump($stmt->fetchAll(PDO::FETCH_NUM));

unset($stmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec('DROP TABLE gh23758');
unset($dbh);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
  }
}
