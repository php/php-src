--TEST--
Bug #27597 (pg_fetch_array not returning false)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('inc/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__ . '/inc/config.inc');

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}

pg_query($dbh, "CREATE TABLE id (id INT)");

for ($i=0; $i<4; $i++) {
    pg_query($dbh, "INSERT INTO id (id) VALUES ($i)");
}

function xi_fetch_array($res, $type = PGSQL_ASSOC) {
    $a = pg_fetch_array($res, NULL, $type) ;
    return $a ;
}

$res = pg_query($dbh, "SELECT * FROM id");
$i = 0; // endless-loop protection
while($row = xi_fetch_array($res)) {
    print_r($row);
    if ($i++ > 4) {
        echo "ENDLESS-LOOP";
        exit(1);
    }
}

pg_close($dbh);

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$dbh = pg_connect($conn_str);

pg_query($dbh, "DROP TABLE id");
?>
--EXPECT--
Array
(
    [id] => 0
)
Array
(
    [id] => 1
)
Array
(
    [id] => 2
)
Array
(
    [id] => 3
)
