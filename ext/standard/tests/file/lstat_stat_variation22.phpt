--TEST--
Test lstat() and stat() functions: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/
echo "*** testing stat ***\n";
var_dump(stat(NULL));
var_dump(stat(false));
var_dump(stat(''));
var_dump(stat(' '));
var_dump(stat('|'));

echo "*** testing lstat ***\n";
var_dump(lstat(NULL));
var_dump(lstat(false));
var_dump(lstat(''));
var_dump(lstat(' '));
var_dump(lstat('|'));
echo "Done";
?>
--EXPECTF--
*** testing stat ***
bool(false)
bool(false)
bool(false)

Warning: stat() expects parameter 1 to be a valid path, string given in %slstat_stat_variation22.php on line %d
NULL

Warning: stat(): stat failed for | in %slstat_stat_variation22.php on line %d
bool(false)
*** testing lstat ***
bool(false)
bool(false)
bool(false)

Warning: lstat() expects parameter 1 to be a valid path, string given in %slstat_stat_variation22.php on line %d
NULL

Warning: lstat(): Lstat failed for | in %slstat_stat_variation22.php on line %d
bool(false)
Done
