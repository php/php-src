--TEST--
Test filesize() function: usage variations - size of files
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* 
 * Prototype   : int filesize ( string $filename );
 * Description : Returns the size of the file in bytes, or FALSE 
 *               (and generates an error of level E_WARNING) in case of an error.
 */

echo "*** Testing filesize(): usage variations ***\n"; 

/* null, false, "", " " */
var_dump( filesize(NULL) );
var_dump( filesize(false) );
var_dump( filesize('') );
var_dump( filesize(' ') );
var_dump( filesize('|') );
echo "*** Done ***\n";
?>
--EXPECTF--	
*** Testing filesize(): usage variations ***
bool(false)
bool(false)
bool(false)

Warning: filesize(): stat failed for   in %s on line %d
bool(false)

Warning: filesize(): stat failed for | in %s on line %d
bool(false)
*** Done ***
