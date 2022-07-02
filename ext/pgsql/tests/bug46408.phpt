--TEST--
Bug #46408 (Locale number format settings can cause pg_query_params to break with numerics)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('skipif.inc');
if (false === setlocale(LC_ALL, "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8")) {
    echo "skip Locale de-DE not present";
}
?>
--FILE--
<?php

require_once('config.inc');

$dbh = pg_connect($conn_str);
setlocale(LC_ALL, "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8");
echo 3.5 , "\n";
pg_query_params($dbh, "SELECT $1::numeric", array(3.5));
pg_close($dbh);

echo "Done".PHP_EOL;

?>
--EXPECT--
3.5
Done
