--TEST--
PDO_OCI: Attribute: Setting session action
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc');

$query = 'select action from v$session where sid = (select distinct sid from v$mystat)';

$dbh = PDOTest::factory();

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION NOT SET: ';
var_dump($row['action']);

$dbh->setAttribute(PDO::OCI_ATTR_ACTION, "some action");

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION SET: ';
var_dump($row['action']);

$dbh->setAttribute(PDO::OCI_ATTR_ACTION, "something else!");

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION RESET: ';
var_dump($row['action']);

$dbh->setAttribute(PDO::OCI_ATTR_ACTION, null);

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION NULLED: ';
var_dump($row['action']);

echo "Done\n";

?>
--EXPECT--
ACTION NOT SET: NULL
ACTION SET: string(11) "some action"
ACTION RESET: string(15) "something else!"
ACTION NULLED: NULL
Done
