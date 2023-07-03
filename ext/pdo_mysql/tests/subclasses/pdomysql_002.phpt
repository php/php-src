--TEST--
PDO_mysql connect through PDO::connect
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoMysql::class) === false) {
    echo "PdoMysql class does not exist.\n";
    exit(-1);
}

echo "PdoMysql class exists.\n";

[$dsn, $user, $pass] = getDsnUserAndPassword();


$db =  Pdo::connect($dsn, $user, $pass);

if (!$db instanceof PdoMysql) {
    echo "Wrong class type. Should be PdoMysql but is [" . get_class($db) . "\n";
}

$db->query('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE IF NOT EXISTS test(id int NOT NULL PRIMARY KEY, name VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A')");
$db->exec("INSERT INTO test VALUES(2, 'B')");
$db->exec("INSERT INTO test VALUES(3, 'C')");

foreach ($db->query('SELECT name FROM test') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE test');

echo "Fin.";
?>
--EXPECT--
PdoMysql class exists.
array(2) {
  ["name"]=>
  string(1) "A"
  [0]=>
  string(1) "A"
}
array(2) {
  ["name"]=>
  string(1) "B"
  [0]=>
  string(1) "B"
}
array(2) {
  ["name"]=>
  string(1) "C"
  [0]=>
  string(1) "C"
}
Fin.
