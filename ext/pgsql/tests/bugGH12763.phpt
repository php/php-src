--TEST--
Bug #GH12763 (pg_untrace(): Argument #1 ($connection) must be of type resource or null, PgSql\Connection given)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

$conn = pg_connect($conn_str);

pg_untrace($conn);
echo "OK";

?>
--EXPECT--
OK
