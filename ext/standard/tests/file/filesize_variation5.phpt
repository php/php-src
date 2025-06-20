--TEST--
Test filesize() function: usage variations - size of files
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing filesize(): usage variations ***\n";

var_dump( filesize('') );
var_dump( filesize(' ') );
var_dump( filesize('|') );
echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing filesize(): usage variations ***
bool(false)

Warning: filesize(): stat failed for   in %s on line %d
bool(false)

Warning: filesize(): stat failed for | in %s on line %d
bool(false)
*** Done ***
