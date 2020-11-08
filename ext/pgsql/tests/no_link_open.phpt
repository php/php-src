--TEST--
Using pg function with default link while no link open
--FILE--
<?php

try {
    pg_dbname();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
No PostgreSQL link opened yet
