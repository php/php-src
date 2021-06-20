--TEST--
PDO_Firebird: bug 53280 segfaults if query column count is less than param count
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
@$dbh->exec('DROP TABLE testz');
$dbh->exec('CREATE TABLE testz(A VARCHAR(30), B VARCHAR(30), C VARCHAR(30))');
$dbh->exec("INSERT INTO testz VALUES ('A', 'B', 'C')");
$dbh->commit();

$stmt1 = "SELECT B FROM testz WHERE A = ? AND B = ?";
$stmt2 = "SELECT B, C FROM testz WHERE A = ? AND B = ?";

$stmth2 = $dbh->prepare($stmt2);
$stmth2->execute(array('A', 'B'));
$rows = $stmth2->fetchAll(); // <------ OK
var_dump($rows);

$stmth1 = $dbh->prepare($stmt1);
$stmth1->execute(array('A', 'B'));
$rows = $stmth1->fetchAll(); // <------- segfault
var_dump($rows);

$dbh->commit();
unset($stmth1);
unset($stmth2);

$dbh->exec('DROP TABLE testz');

unset($stmt);
unset($dbh);

?>
--EXPECT--
array(1) {
  [0]=>
  array(4) {
    ["B"]=>
    string(1) "B"
    [0]=>
    string(1) "B"
    ["C"]=>
    string(1) "C"
    [1]=>
    string(1) "C"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["B"]=>
    string(1) "B"
    [0]=>
    string(1) "B"
  }
}
