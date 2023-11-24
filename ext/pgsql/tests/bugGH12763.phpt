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

$file_name = tempnam('.', 'trace.log');
pg_trace($file_name, 'w', $conn);
pg_query($conn, 'select 1 as a');
pg_untrace($conn);
$items = explode("\n", file_get_contents($file_name));
foreach ($items as &$item) {
    $item = preg_replace('!^.*?\s!', '', $item);
    $item = preg_replace('!^.*?\s!', '', $item);
}
unlink($file_name);
echo md5(implode("\n", $items));

?>
--EXPECT--
487dba3a362f8aa1c4b585cdd87d3f2d