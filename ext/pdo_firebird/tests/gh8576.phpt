--TEST--
GH-8576 (Bad interpretation of length when char is UTF-8)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require 'testdb.inc';

$dbh->exec("CREATE TABLE gh8576 (name CHAR(1) CHARACTER SET UTF8)");
$dbh->exec("INSERT INTO gh8576 VALUES ('A')");
$stmt = $dbh->query("SELECT * FROM gh8576");
var_dump($stmt->fetchAll());
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["NAME"]=>
    string(1) "A"
    [0]=>
    string(1) "A"
  }
}
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE gh8576");
unset($dbh);
?>
