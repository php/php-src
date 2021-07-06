--TEST--
PDO_OCI: Attribute: Setting session action
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

$query = 'select action from v$session where sid = sys_context(\'USERENV\', \'SID\')';

$dbh = PDOTest::factory();

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION NOT SET: ';
var_dump($row['action']);

var_dump($dbh->setAttribute(PDO::OCI_ATTR_ACTION, "some action"));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION SET: ';
var_dump($row['action']);

var_dump($dbh->setAttribute(PDO::OCI_ATTR_ACTION, "something else!"));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION RESET: ';
var_dump($row['action']);

var_dump($dbh->setAttribute(PDO::OCI_ATTR_ACTION, null));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'ACTION NULLED: ';
var_dump($row['action']);

echo "Done\n";

?>
--EXPECT--
ACTION NOT SET: NULL
bool(true)
ACTION SET: string(11) "some action"
bool(true)
ACTION RESET: string(15) "something else!"
bool(true)
ACTION NULLED: NULL
Done
