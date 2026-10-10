--TEST--
pg_query_params(), pg_execute() and the escape functions must reject null bytes
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
$value = "hello\0world";
$sql = 'SELECT CAST($1 AS text), CAST($2 AS text), CAST($3 AS text)';
$params = ['ok', null, $value];

pg_prepare($db, 'null_bytes_stmt', $sql);

$calls = [
    'pg_query_params' => fn() => pg_query_params($db, $sql, $params),
    'pg_execute' => fn() => pg_execute($db, 'null_bytes_stmt', $params),
    'pg_send_query_params' => fn() => pg_send_query_params($db, $sql, $params),
    'pg_send_execute' => fn() => pg_send_execute($db, 'null_bytes_stmt', $params),
    'pg_escape_string' => fn() => pg_escape_string($db, $value),
    'pg_escape_literal' => fn() => pg_escape_literal($db, $value),
    'pg_escape_identifier' => fn() => pg_escape_identifier($db, $value),
];

foreach ($calls as $call) {
    try {
        var_dump($call());
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

var_dump(pg_escape_bytea($db, $value));
var_dump(pg_fetch_row(pg_query_params($db, $sql, ['a', null, 'c'])));
?>
--EXPECT--
ValueError: pg_query_params(): Argument #3 ($params) must not contain strings with any null bytes
ValueError: pg_execute(): Argument #3 ($params) must not contain strings with any null bytes
ValueError: pg_send_query_params(): Argument #3 ($params) must not contain strings with any null bytes
ValueError: pg_send_execute(): Argument #3 ($params) must not contain strings with any null bytes
ValueError: pg_escape_string(): Argument #2 ($string) must not contain any null bytes
ValueError: pg_escape_literal(): Argument #2 ($string) must not contain any null bytes
ValueError: pg_escape_identifier(): Argument #2 ($string) must not contain any null bytes
string(24) "\x68656c6c6f00776f726c64"
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  NULL
  [2]=>
  string(1) "c"
}
