--TEST--
Test lstat() and stat() functions: usage variations - invalid filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip ... not for Windows');
}
?>
--CONFLICTS--
obscure_filename
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
?>
--EXPECTF--
*** testing stat ***
bool(false)
bool(false)
bool(false)

Warning: stat(): stat failed for   in %s on line %d
bool(false)

Warning: stat(): stat failed for | in %s on line %d
bool(false)
*** testing lstat ***
bool(false)
bool(false)
bool(false)

Warning: lstat(): Lstat failed for   in %s on line %d
bool(false)

Warning: lstat(): Lstat failed for | in %s on line %d
bool(false)
