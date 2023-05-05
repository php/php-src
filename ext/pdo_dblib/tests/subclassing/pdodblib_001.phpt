--TEST--
PdoDblib basic
--EXTENSIONS--
pdo
--FILE--
<?php

require_once __DIR__ . "/../config_functions.php";

if (class_exists(PdoDblib::class) === false) {
    echo "PdoDblib class does not exist.\n";
    exit(-1);
}
echo "PdoDblib class exists.\n";


[$dsn, $user, $pass] = getDsnUserAndPassword();

$db = new PdoDblib($dsn, $user, $pass);

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');

foreach ($db->query('SELECT testing(name) FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

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
Fin.
