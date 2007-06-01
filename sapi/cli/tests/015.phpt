--TEST--
CLI long options
--SKIPIF--
<?php 
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');


echo `"$php" --version | grep built:`;
echo `echo "<?php print_r(\\\$argv);" | "$php" -- foo bar baz`, "\n";
echo `"$php" --version foo bar baz | grep built:`;
echo `"$php" --notexisting foo bar baz | grep Usage:`;

echo "Done\n";
?>
--EXPECTF--     
PHP %d.%d.%d%s(cli) (built: %s)%s
Array
(
    [0] => -
    [1] => foo
    [2] => bar
    [3] => baz
)

PHP %d.%d.%d%s(cli) (built: %s)%s
Usage: %s [options] [-f] <file> [--] [args...]
Done
