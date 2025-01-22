--TEST--
GH-17158 (pg_fetch_result Shows Incorrect ArgumentCountError Message when Called With 1 Argument)
--EXTENSIONS--
pgsql
--FILE--
<?php

try {
    pg_fetch_result(null);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
pg_fetch_result() expects at least 2 arguments, 1 given
