--TEST--
PDO_firebird coonect through PDO::connect
--EXTENSIONS--
PDO_odbc
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoFirebird::class) === false) {
    echo "PdoFirebird class does not exist.\n";
    exit(-1);
}

echo "PdoFirebird class exists.\n";

[$dsn, $user, $pass] = getDsnUserAndPassword();

$db =  Pdo::connect($dsn, $user, $pass);

if (!$db instanceof PdoFirebird) {
    echo "Wrong class type. Should be PdoOdbc but is [" . get_class($db) . "\n";
}

$db->query('RECREATE TABLE test (idx int NOT NULL PRIMARY KEY, name VARCHAR(20))');

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
PdoFirebird class exists.
array(2) {
  ["NAME"]=>
  string(1) "A"
  [0]=>
  string(1) "A"
}
array(2) {
  ["NAME"]=>
  string(1) "B"
  [0]=>
  string(1) "B"
}
array(2) {
  ["NAME"]=>
  string(1) "C"
  [0]=>
  string(1) "C"
}
Fin.
