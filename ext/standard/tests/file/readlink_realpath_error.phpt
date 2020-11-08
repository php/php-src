--TEST--
Test readlink() and realpath() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for Windows');
}
?>
--FILE--
<?php
echo "\n*** Testing readlink() on a non-existent link ***\n";
var_dump( readlink(__DIR__."/readlink_error.tmp") );

echo "\n*** Testing readlink() on existing file ***\n";
var_dump( readlink(__FILE__) );

echo "\n*** Testing readlink() on existing directory ***\n";
var_dump( readlink(__DIR__) );

echo "\n*** Testing realpath() on a non-existent file ***\n";
var_dump( realpath(__DIR__."/realpath_error.tmp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing readlink() on a non-existent link ***

Warning: readlink(): No such file or directory in %s on line %d
bool(false)

*** Testing readlink() on existing file ***

Warning: readlink(): Invalid argument in %s on line %d
bool(false)

*** Testing readlink() on existing directory ***

Warning: readlink(): Invalid argument in %s on line %d
bool(false)

*** Testing realpath() on a non-existent file ***
%s
Done
