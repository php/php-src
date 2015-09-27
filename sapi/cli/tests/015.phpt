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


echo `"$php" -n --version | grep ^PHP`;
echo `echo "<?php print_r(\\\$argv);" | "$php" -n -- foo bar baz`, "\n";
echo `"$php" -n --version foo bar baz | grep ^PHP`;
echo `"$php" -n --notexisting foo bar baz | grep Usage:`;

echo "Done\n";
?>
--EXPECTF--     
PHP %d.%d.%d%s(cli)%s
Array
(
    [0] => -
    [1] => foo
    [2] => bar
    [3] => baz
)

PHP %d.%d.%d%s(cli)%s
Usage: %s [options] [-f] <file> [--] [args...]
Done
