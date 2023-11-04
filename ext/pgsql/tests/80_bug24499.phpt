--TEST--
Bug #24499 (Notice: Undefined property: stdClass::)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('inc/skipif.inc');
?>
--FILE--
<?php

require_once('inc/config.inc');
$table_name = 'table_80_bug24499';

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}

pg_query($dbh, "CREATE TABLE {$table_name} (id SERIAL, t INT)");

for ($i=0; $i<4; $i++) {
    pg_query($dbh, "INSERT INTO {$table_name} (t) VALUES ($i)");
}

class Id
{
    public $id;

    public function getId()
    {
        global $dbh;
        global $table_name;

        $q  = pg_query($dbh, "SELECT id FROM {$table_name}");
        print_r(pg_fetch_array($q));
        print_r(pg_fetch_array($q));
        $id = pg_fetch_object($q);
        var_dump($id);
        return $id->id;
    }
}

$id = new Id();
var_dump($id->getId());

pg_close($dbh);

echo "Done\n";

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_80_bug24499';

$dbh = pg_connect($conn_str);
pg_query($dbh, "DROP TABLE {$table_name} CASCADE");
?>
--EXPECTF--
Array
(
    [0] => 1
    [id] => 1
)
Array
(
    [0] => 2
    [id] => 2
)
object(stdClass)#%d (1) {
  ["id"]=>
  string(1) "3"
}
string(1) "3"
Done
