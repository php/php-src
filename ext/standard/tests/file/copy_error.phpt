--TEST--
Test copy() function: error conditions
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
 * Description: Makes a copy of the file source to dest.
 *              Returns TRUE on success or FALSE on failure.
 */

echo "*** Testing copy() function: error conditions --\n";
/* Invalid args */
var_dump( copy("/no/file", "file") );

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing copy() function: error conditions --

Warning: copy(/no/file): failed to open stream: No such file or directory in %s on line %d
bool(false)
*** Done ***
