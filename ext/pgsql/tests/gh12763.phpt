--TEST--
Bug #GH12763 (pg_untrace(): Argument #1 ($connection) must be of type resource or null, PgSql\Connection given)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

$conn = pg_connect($conn_str);

$file_name = tempnam('.', 'trace.log');
pg_trace($file_name, 'w', $conn);
pg_query($conn, 'select 1 as a');
pg_untrace($conn);
$items = explode("\n", file_get_contents($file_name));
unlink($file_name);
echo isset($items[0]) ? 'OK' : 'FAIL';

?>
--EXPECT--
OK
