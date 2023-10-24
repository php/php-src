--TEST--
PDO_DBLIB: Out of memory on large recordsets
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip May fail on repeat');
require __DIR__ . '/config.inc';
?>
--CONFLICTS--
all
--FILE--
<?php
require __DIR__ . '/config.inc';

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
