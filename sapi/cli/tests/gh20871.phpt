--TEST--
GH-20871: Bad error line and file for invalid "-d" arguments
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

exec("$php -d 'foo=bar=asd' -r \"echo 'hello' . PHP_EOL;\" 2>&1", $output, $exit_code);
print_r($output);

echo "\n";

exec("$php -d 'foo=bar!asd' -r \"echo 'world' . PHP_EOL;\" 2>&1", $output2, $exit_code2);
print_r($output2);

?>
--EXPECTF--
Array
(
    [0] => PHP:  syntax error, unexpected '=' in INI command line parameter '-d'
    [1] => hello
)

Array
(
    [0] => PHP:  syntax error, unexpected '!' in INI command line parameter '-d'
    [1] => world
)
