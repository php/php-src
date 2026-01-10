--TEST--
GH-20871: Bad error line and file for invalid "-d" arguments
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

exec("$php -d 'foo=bar=asd' -r \"echo 'hello' . PHP_EOL;\" 2>&1", $output, $exit_code);
print_r($output);

?>
--EXPECTF--
Array
(
    [0] => PHP:  syntax error, unexpected '=' in -d option on line 0
    [1] => hello
)
