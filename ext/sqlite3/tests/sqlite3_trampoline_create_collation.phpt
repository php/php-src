--TEST--
Test SQLite3::createCollation() trampoline callback
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once __DIR__ .  '/new_db.inc';

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return strnatcmp(...$arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'NAT'];

$db->createCollation('NAT', $callback);

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
Trampoline for NAT
Trampoline for NAT
default
a1
a10
a2
natural
Trampoline for NAT
Trampoline for NAT
a1
a2
a10
