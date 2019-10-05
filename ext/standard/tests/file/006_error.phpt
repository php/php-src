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
/*
  Prototype: int fileperms ( string $filename )
  Description: Returns the permissions on the file, or FALSE in case of an error

  Prototype: bool chmod ( string $filename, int $mode )
  Description: Attempts to change the mode of the file specified by
    filename to that given in mode
*/

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

/* With args less than expected */
$fp = fopen(__DIR__."/006_error.tmp", "w");
fclose($fp);
var_dump( chmod(__DIR__."/006_error.tmp") );
var_dump( chmod("nofile") );
var_dump( chmod() );
var_dump( fileperms() );

/* With args greater than expected */
var_dump( chmod(__DIR__."/006_error.tmp", 0755, TRUE) );
var_dump( fileperms(__DIR__."/006_error.tmp", 0777) );
var_dump( fileperms("nofile", 0777) );

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


Warning: chmod() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: chmod() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: chmod() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: fileperms() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: chmod() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: fileperms() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: fileperms() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Done ***
