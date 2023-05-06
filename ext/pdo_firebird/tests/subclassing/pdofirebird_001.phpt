--TEST--
PDO_firebird subclass basic
--EXTENSIONS--
pdo_firebird
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PDOFirebird::class) === false) {
    echo "PDOFirebird class does not exist.\n";
    exit(-1);
}
echo "PDOFirebird class exists.\n";


[$dsn, $user, $pass] = getDsnUserAndPassword();

$db = new PDOFirebird($dsn, $user, $pass);

try {
    $db->query('DROP TABLE foobar');
} catch (\Exception $e) {
    // TODO - find the 'not exists' syntax.
}

$db->query('CREATE TABLE foobar (idx int NOT NULL PRIMARY KEY, name VARCHAR(20))');
$db->query("INSERT INTO foobar VALUES (1, 'PHP')");
$db->query("INSERT INTO foobar VALUES (2, 'PHP6')");

foreach ($db->query('SELECT name FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

echo "Fin.";
?>
--EXPECT--
PDOFirebird class exists.
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
