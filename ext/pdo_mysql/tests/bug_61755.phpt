--TEST--
Bug #61755 (A parsing bug in the prepared statements can lead to access violations)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "NULL-Byte before first placeholder:\n";
$s = $db->prepare("SELECT \"a\0b\", ?");
$s->bindValue(1,"c");
$s->execute();
$r = $s->fetch();
echo "Length of item 0: ".strlen($r[0]).", Value of item 1: ".$r[1]."\n";

echo "\nOpen comment:\n";
try {
    $s = $db->prepare("SELECT /*");
    $s->execute();
} catch (Exception $e) {
    echo "Error code: ".$e->getCode()."\n";
}

echo "\ndone!\n";
?>
--EXPECTF--
NULL-Byte before first placeholder:
Length of item 0: 3, Value of item 1: c

Open comment:
Error code: 42000

done!
