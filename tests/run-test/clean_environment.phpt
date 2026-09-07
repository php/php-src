--TEST--
SKIPIF and CLEAN do not inherit request environment variables on POSIX
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
    foreach (['REQUEST_METHOD', 'QUERY_STRING', 'PATH_TRANSLATED', 'SCRIPT_FILENAME'] as $name) {
        if (getenv($name) !== false) {
            die("$name was inherited\n");
        }
    }
}
?>
--FILE--
<?php
?>
--CLEAN--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
    foreach (['REQUEST_METHOD', 'QUERY_STRING', 'PATH_TRANSLATED', 'SCRIPT_FILENAME'] as $name) {
        if (getenv($name) !== false) {
            echo "$name was inherited\n";
        }
    }
}
?>
--EXPECT--
