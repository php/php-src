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
--EXPECT--
No PostgreSQL connection opened yet
