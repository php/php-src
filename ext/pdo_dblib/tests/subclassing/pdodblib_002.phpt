--TEST--
PdoDblib create through PDO::connect
--EXTENSIONS--
pdo
--FILE--
<?php

if (class_exists(PdoDblib::class) === false) {
    echo "PdoDblib class does not exist.\n";
    exit(-1);
}

echo "PdoDblib class exists.\n";

$db = Pdo::connect('dblib::memory:');

$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A')");
$db->exec("INSERT INTO test VALUES(2, 'B')");
$db->exec("INSERT INTO test VALUES(3, 'C')");

$stmt = $db->prepare('SELECT * from test');
$stmt->execute();

var_dump($stmt->fetchAll(PDO::FETCH_OBJ));


$db->query('DROP TABLE foobar');

echo "Fin.";
?>
--EXPECT--
PdoDblib class exists.
array(2) {
  ["testing(name)"]=>
  string(3) "php"
  [0]=>
  string(3) "php"
}
array(2) {
  ["testing(name)"]=>
  string(4) "php6"
  [0]=>
  string(4) "php6"
}
Fin.
