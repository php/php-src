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

$db->query('drop table if exists #foobar;');

$db->query("create table #foobar(name varchar(32)); ");
$db->query("insert into #foobar values('PHP');");
$db->query("insert into #foobar values('PHP6');");

foreach ($db->query('SELECT name FROM #foobar') as $row) {
    var_dump($row);
}

$db->query('drop table #foobar;');

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
