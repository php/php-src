--TEST--
PDO_oci subclass basic
--EXTENSIONS--
pdo_oci
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoOci::class) === false) {
    echo "PdoOci class does not exist.\n";
    exit(-1);
}
echo "PdoOci class exists.\n";

[$dsn, $user, $pass] = getDsnUserAndPassword();

$db = new PdoOci($dsn, $user, $pass);

$db->query('CREATE TABLE IF NOT EXISTS foobar(id NUMBER, name VARCHAR2(6))');

$db->query("INSERT INTO foobar VALUES (NULL, 'PHP')");
$db->query("INSERT INTO foobar VALUES (NULL, 'PHP6')");

foreach ($db->query('SELECT name FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

echo "Fin.";
?>
--EXPECT--
PdoOci class exists.
array(2) {
  ["NAME"]=>
  string(3) "PHP"
  [0]=>
  string(3) "PHP"
}
array(2) {
  ["NAME"]=>
  string(4) "PHP6"
  [0]=>
  string(4) "PHP6"
}
Fin.
