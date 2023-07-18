--TEST--
PdoSqlite create through PDO::connect and function define.
--EXTENSIONS--
pdo
--FILE--
<?php

if (class_exists(PdoSqlite::class) === false) {
    echo "PdoSqlite class does not exist.\n";
    exit(-1);
}

echo "PdoSqlite class exists.\n";

$db = Pdo::connect('sqlite::memory:');

if (!$db instanceof PdoSqlite) {
    echo "Wrong class type. Should be PdoSqlite but is [" . get_class($db) . "\n";
}

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT AUTO INCREMENT, name TEXT)');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');


$db->createFunction('testing', function($v) { return strtolower($v); }, 1, PdoSqlite::DETERMINISTIC);


foreach ($db->query('SELECT testing(name) FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

try {
    $db->createFunction('not_going_to_work', function($v) { return strtolower($v); }, -5, PdoSqlite::DETERMINISTIC);
    echo "Failed to throw exception with invalid parameter.\n";
}
catch (ValueError $ve) {
    echo $ve->getMessage() . "\n";
}

echo "Fin.";
?>
--EXPECT--
PdoSqlite class exists.
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
PdoSqlite::createFunction(): Argument #3 ($num_args) must be either -1 (for any number of parameters) or greater than equal to zero.
Fin.
