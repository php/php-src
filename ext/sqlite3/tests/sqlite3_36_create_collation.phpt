--TEST--
Test SQLite3::createCollation() by adding strnatcmp() as an SQL COLLATE sequence
--SKIPIF--
<?php require_once __DIR__ .  '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ .  '/new_db.inc';

$db->createCollation('NAT', 'strnatcmp');

$db->exec('CREATE TABLE t (s varchar(4))');

$stmt = $db->prepare('INSERT INTO t VALUES (?)');
foreach(array('a1', 'a10', 'a2') as $s){
    $stmt->bindParam(1, $s);
    $stmt->execute();
}

$defaultSort = $db->query('SELECT s FROM t ORDER BY s');             //memcmp() sort
$naturalSort = $db->query('SELECT s FROM t ORDER BY s COLLATE NAT'); //strnatcmp() sort

echo "default\n";
while ($row = $defaultSort->fetchArray()){
    echo $row['s'], "\n";
}

echo "natural\n";
while ($row = $naturalSort->fetchArray()){
    echo $row['s'], "\n";
}

$db->close();

?>
--EXPECT--
default
a1
a10
a2
natural
a1
a2
a10
