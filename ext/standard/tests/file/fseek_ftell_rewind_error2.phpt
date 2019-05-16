--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - ftell()
--FILE--
<?php

/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

echo "*** Testing ftell() : error conditions ***\n";

// ftell on a file handle which is already closed
echo "-- Testing ftell with closed/unset file handle --";
$fp = fopen(__FILE__, "r");
fclose($fp);
var_dump(ftell($fp));

echo "Done\n";
?>
--EXPECTF--
*** Testing ftell() : error conditions ***
-- Testing ftell with closed/unset file handle --
Warning: ftell(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
Done
