--TEST--
Test readlink() and realpath() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
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

Warning: readlink(): readlink failed to read the symbolic link (%s), error %d in %s on line %d
bool(false)

*** Testing readlink() on existing file ***
string(%d) "%s%eext%estandard%etests%efile%ereadlink_realpath_error-win32.php"

*** Testing readlink() on existing directory ***
string(%d) "%s%eext%estandard%etests%efile"

*** Testing realpath() on a non-existent file ***
%s
Done
