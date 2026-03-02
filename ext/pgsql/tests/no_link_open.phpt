--TEST--
Using pg function with default link while no link open
--EXTENSIONS--
pgsql
--FILE--
<?php

try {
    pg_dbname();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: pg_dbname(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
No PostgreSQL connection opened yet
