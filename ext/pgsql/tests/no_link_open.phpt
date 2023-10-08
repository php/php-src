--TEST--
Using pg function with default link while no link open
--EXTENSIONS--
pgsql
--FILE--
<?php

try {
    pg_dbname();
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
pg_dbname() expects exactly 1 argument, 0 given
