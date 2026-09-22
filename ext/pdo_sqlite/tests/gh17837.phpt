--TEST--
GH-17837 (::getColumnMeta() on unexecuted statement segfaults)
--EXTENSIONS--
pdo_sqlite
--CREDITS--
YuanchengJiang
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$stmt = $db->prepare('select :a, :b, ?');
var_dump($stmt->getColumnMeta(0));
?>
--EXPECT--
bool(false)
