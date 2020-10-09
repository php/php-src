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

/* With standard files and dirs */
var_dump( chmod("/etc/passwd", 0777) );
printf("%o", fileperms("/etc/passwd") );
echo "\n";
clearstatcache();

var_dump( chmod("/etc", 0777) );
printf("%o", fileperms("/etc") );
echo "\n";
clearstatcache();

/* With non-existing file or dir */
var_dump( chmod("/no/such/file/dir", 0777) );
var_dump( fileperms("/no/such/file/dir") );
echo "\n";

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
unlink( __DIR__."/006_error.tmp");
?>
--EXPECTF--
*** Testing error conditions for fileperms(), chmod() ***

Warning: chmod(): %s in %s on line %d
bool(false)
100%d44

Warning: chmod(): %s in %s on line %d
bool(false)
40755

Warning: chmod(): No such file or directory in %s on line %d
bool(false)

Warning: fileperms(): stat failed for /no/such/file/dir in %s on line %d
bool(false)


*** Done ***
