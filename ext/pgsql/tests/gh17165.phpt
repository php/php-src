--TEST--
Fix pg_query()/pg_query_params()/pg_prepare()/pg_execute()/pg_set_error_verbosity()/pg_set_client_encoding()/pg_put_line() pg field infos calls ArgumentCountError message.
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php


include "inc/config.inc";

$db = pg_connect($conn_str);
try {
    pg_query("a", "b", "c");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_query_params($db, "b", array(), "d");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_prepare($db, "a", "b", "c");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_set_error_verbosity($db, 0, PHP_INT_MAX);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_set_client_encoding($db, "foo", "bar");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_put_line($db, "my", "data");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    pg_field_is_null($db, false, "myfield", new stdClass());
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
pg_query() expects at most 2 arguments, 3 given
pg_query_params() expects at most 3 arguments, 4 given
pg_prepare() expects at most 3 arguments, 4 given
pg_set_error_verbosity() expects at most 2 arguments, 3 given
pg_set_client_encoding() expects at most 2 arguments, 3 given
pg_put_line() expects at most 2 arguments, 3 given
pg_field_is_null() expects at most 3 arguments, 4 given
