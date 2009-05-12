--TEST--
Bug #44861 (scrollable cursor don't work with pgsql)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$dbh = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$query = "SELECT 'row1' AS r UNION SELECT 'row2' UNION SELECT 'row3' UNION SELECT 'row4'";
$aParams = array(PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL);

$res = $dbh->prepare($query, $aParams);
$res->execute();
var_dump($res->fetchColumn());
var_dump($res->fetchColumn());
var_dump($res->fetchColumn());
var_dump($res->fetchColumn());
var_dump($res->fetchColumn());

var_dump($res->fetch(PDO::FETCH_NUM, PDO::FETCH_ORI_ABS, 3));
var_dump($res->fetch(PDO::FETCH_NUM, PDO::FETCH_ORI_PRIOR));
var_dump($res->fetch(PDO::FETCH_NUM, PDO::FETCH_ORI_FIRST));
var_dump($res->fetch(PDO::FETCH_NUM, PDO::FETCH_ORI_LAST));
var_dump($res->fetch(PDO::FETCH_NUM, PDO::FETCH_ORI_REL, -1));

var_dump($res->fetchAll(PDO::FETCH_ASSOC));

// Test binding params via emulated prepared query
$res = $dbh->prepare("SELECT ?", $aParams);
$res->execute(array("it's working"));
var_dump($res->fetch(PDO::FETCH_NUM));


// Test bug #48188, trying to execute again
$res->execute(array("try again"));
var_dump($res->fetchColumn());
var_dump($res->fetchColumn());

?>
--EXPECT--
string(4) "row1"
string(4) "row2"
string(4) "row3"
string(4) "row4"
bool(false)
array(1) {
  [0]=>
  string(4) "row3"
}
array(1) {
  [0]=>
  string(4) "row2"
}
array(1) {
  [0]=>
  string(4) "row1"
}
array(1) {
  [0]=>
  string(4) "row4"
}
array(1) {
  [0]=>
  string(4) "row3"
}
array(1) {
  [0]=>
  array(1) {
    ["r"]=>
    string(4) "row4"
  }
}
array(1) {
  [0]=>
  string(12) "it's working"
}
string(9) "try again"
bool(false)
