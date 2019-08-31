--TEST--
PDO_OCI: Attribute: Column Case
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

function do_query1($dbh)
{
	var_dump($dbh->getAttribute(PDO::ATTR_CASE));
	$s = $dbh->prepare("select dummy from dual");
	$s->execute();
	while ($r = $s->fetch(PDO::FETCH_ASSOC)) {
		var_dump($r);
	}
}

function do_query2($dbh, $mode)
{
	echo "Mode desired is $mode\n";
	$s = $dbh->prepare("select dummy from dual", array(PDO::ATTR_CASE, $mode));
	$s->execute();
	while ($r = $s->fetch(PDO::FETCH_ASSOC)) {
		var_dump($r);
	}
}

$dbh = PDOTest::factory();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Test 1 - Force column names to lower case\n";
$dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
do_query1($dbh);

echo "Test 2 - Leave column names as returned by the database driver\n";
$dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);
do_query1($dbh);

echo "Test 3 - Force column names to upper case\n";
$dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_UPPER);
do_query1($dbh);

echo "Test 4 - Setting on statement has no effect.  Attempt lower case but get upper\n";
$dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL); // reset
do_query2($dbh, PDO::CASE_LOWER);

echo "Done\n";

?>
--EXPECT--
Test 1 - Force column names to lower case
int(2)
array(1) {
  ["dummy"]=>
  string(1) "X"
}
Test 2 - Leave column names as returned by the database driver
int(0)
array(1) {
  ["DUMMY"]=>
  string(1) "X"
}
Test 3 - Force column names to upper case
int(1)
array(1) {
  ["DUMMY"]=>
  string(1) "X"
}
Test 4 - Setting on statement has no effect.  Attempt lower case but get upper
Mode desired is 2
array(1) {
  ["DUMMY"]=>
  string(1) "X"
}
Done
