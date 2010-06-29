--TEST--
PDO_DBLIB: Out of memory on large recordsets
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
try {
	$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
} catch (PDOException $e) {
	die('skip ' . $e->getMessage());
}
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

/* This should be sufficient to overflow any buffers */
$stmt = $db->prepare("select *
from information_schema.columns ic1
cross join information_schema.columns ic2
cross join information_schema.columns ic3");

$x = $stmt->execute();
$n = 0;
while (($r = $stmt->fetch())) {
	$n++;
}
$stmt = null;

echo "OK\n";

?>
--EXPECT--
OK
