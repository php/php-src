--TEST--
CLI php -i extension_dir
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$output = `$php -n -i`;
var_dump(str_contains($output, "extension_dir => "));

?>
--EXPECT--
bool(true)
