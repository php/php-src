--TEST--
Bug #46408 (Locale number format settings can cause pg_query_params to break with numerics)
--SKIPIF--
<?php
require_once('skipif.inc');
if (false === setlocale(LC_ALL, 'de_DE.utf-8', 'de_DE')) {
    echo "skip Locale de_DE.utf-8 not present";
}
?>
--FILE--
<?php

require_once('config.inc');

$dbh = pg_connect($conn_str);
setlocale(LC_ALL, 'de_DE.utf-8', 'de_DE');
echo 3.5 , "\n";
pg_query_params("SELECT $1::numeric", array(3.5));
pg_close($dbh);

echo "Done".PHP_EOL;

?>
--EXPECT--
3.5
Done
