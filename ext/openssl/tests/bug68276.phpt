--TEST--
Fixed Bug #68276 Reproducible memory corruption: openssl extension
--SKIPIF--
<?php include(__DIR__."/../../../ext/pgsql/tests/skipif_ssl.inc"); ?>
--FILE--
<?php
include(__DIR__."/../../../ext/pgsql/tests/config_ssl.inc");

$conn = pg_connect($conn_str);

openssl_pkey_get_public('a');

$sql="SELECT repeat('a', 160000)";

if ($r = pg_query($sql))
        echo "OK\n";
else
        echo pg_last_error($conn);


while ($msg = openssl_error_string())
    echo $msg;


?>
--EXPECT--
OK
error:0906D06C:PEM routines:PEM_read_bio:no start line
