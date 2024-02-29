--TEST--
Test of fileowner() function: error conditions
--FILE--
<?php

echo "*** Testing fileowner(): error conditions ***\n";
/* Non-existing file or dir */
var_dump( fileowner("/no/such/file/dir") );

/* Invalid arguments */
var_dump( fileowner("string") );
var_dump( fileowner(100) );

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing fileowner(): error conditions ***

Warning: fileowner('%s'): stat failed for /no/such/file/dir in %s on line %d
bool(false)

Warning: fileowner('string'): stat failed for string in %s on line %d
bool(false)

Warning: fileowner('100'): stat failed for 100 in %s on line %d
bool(false)

*** Done ***
