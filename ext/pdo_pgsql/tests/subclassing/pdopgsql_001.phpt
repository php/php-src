--TEST--
PdoPgsql subclass basic
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoPgsql::class) === false) {
    echo "PdoPgsql class does not exist.\n";
    exit(-1);
}
echo "PdoPgsql class exists.\n";

$dsn = getDsn();

$db = new PdoPgsql($dsn);

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT, name TEXT)');

$db->query("INSERT INTO foobar VALUES (NULL, 'PHP')");
$db->query("INSERT INTO foobar VALUES (NULL, 'PHP6')");

foreach ($db->query('SELECT name FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

echo "Fin.";
?>
--EXPECT--
PdoPgsql class exists.
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
