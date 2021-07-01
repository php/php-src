--TEST--
PDO_OCI: Attribute: Setting session module
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

$query = 'select module from v$session where sid = sys_context(\'USERENV\', \'SID\')';

$dbh = PDOTest::factory();

var_dump($dbh->setAttribute(PDO::OCI_ATTR_MODULE, "some module"));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'MODULE SET: ';
var_dump($row['module']);

var_dump($dbh->setAttribute(PDO::OCI_ATTR_MODULE, "something else!"));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'MODULE RESET: ';
var_dump($row['module']);

var_dump($dbh->setAttribute(PDO::OCI_ATTR_MODULE, null));

$stmt = $dbh->query($query);
$row = $stmt->fetch();
echo 'MODULE NULLED: ';
var_dump($row['module']);

echo "Done\n";

?>
--EXPECT--
bool(true)
MODULE SET: string(11) "some module"
bool(true)
MODULE RESET: string(15) "something else!"
bool(true)
MODULE NULLED: NULL
Done
