--TEST--
Test fileperms(), chmod() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
echo "*** Testing error conditions for fileperms(), chmod() ***\n";

/* With non-existing file or dir */
var_dump( chmod(__DIR__ . "/no/such/file/dir", 0777) );
var_dump( fileperms(__DIR__ . "/no/such/file/dir") );
echo "\n";

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing error conditions for fileperms(), chmod() ***

Warning: chmod(): %s in %s on line %d
bool(false)

Warning: fileperms(): stat failed for %s/no/such/file/dir in %s on line %d
bool(false)


*** Done ***
