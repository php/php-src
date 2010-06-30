--TEST--
PDO_DBLIB: Out of memory on large recordsets
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

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
