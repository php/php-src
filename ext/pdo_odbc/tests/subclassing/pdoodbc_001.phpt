--TEST--
PDO_odbc subclass basic
--EXTENSIONS--
pdo_odbc
--FILE--
<?php

require_once __DIR__ . "/../config_functions.php";

if (class_exists(PdoOdbc::class) === false) {
    echo "PdoOdbc class does not exist.\n";
    exit(-1);
}
echo "PdoOdbc class exists.\n";


[$dsn, $user, $pass] = getDsnUserAndPassword();

$db = new PdoOdbc($dsn, $user, $pass);

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT, name TEXT)');

$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');

foreach ($db->query('SELECT name FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

echo "Fin.";
?>
--EXPECT--
PdoOdbc class exists.
array(2) {
  ["name"]=>
  string(3) "PHP"
  [0]=>
  string(3) "PHP"
}
array(2) {
  ["name"]=>
  string(4) "PHP6"
  [0]=>
  string(4) "PHP6"
}
Fin.
