--TEST--
PdoDblib create through PDO::connect
--EXTENSIONS--
pdo
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoDblib::class) === false) {
    echo "PdoDblib class does not exist.\n";
    exit(-1);
}

echo "PdoDblib class exists.\n";

[$dsn, $user, $pass] = getDsnUserAndPassword();

$db = Pdo::connect($dsn, $user, $pass);

if (!$db instanceof PdoDblib) {
    echo "Wrong class type. Should be PdoDblib but is [" . get_class($db) . "\n";
}

$db->query('drop table if exists #test;');

$db->query("create table #test(name varchar(32)); ");
$db->query("insert into #test values('PHP');");
$db->query("insert into #test values('PHP6');");

foreach ($db->query('SELECT name FROM #test') as $row) {
    var_dump($row);
}

$db->query('drop table #test;');

echo "Fin.";
?>
--EXPECT--
PdoDblib class exists.
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
