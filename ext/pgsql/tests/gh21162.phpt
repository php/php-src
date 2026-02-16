--TEST--
GH-21162 (pg_connect() on error memory leak)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
    die('skip not for windows');
}
?>
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr) {
    echo "Warning caught\n";
});

pg_connect('');

echo "Done\n";
?>
--EXPECT--
Warning caught
Done
