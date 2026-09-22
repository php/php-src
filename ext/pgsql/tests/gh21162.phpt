--TEST--
GH-21162 (pg_connect() on error memory leak)
--EXTENSIONS--
pgsql
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr) {
    echo "Warning caught\n";
});

pg_connect('host=blablahost.');

echo "Done\n";
?>
--EXPECT--
Warning caught
Done
