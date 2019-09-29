--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - fseek()
--FILE--
<?php

/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

echo "*** Testing fseek() : error conditions ***\n";

// fseek() on a file handle which is already closed
echo "-- Testing fseek() with closed/unset file handle --\n";
$fp = fopen(__FILE__, "r");
fclose($fp);
try {
    var_dump(fseek($fp,10));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing fseek() : error conditions ***
-- Testing fseek() with closed/unset file handle --
fseek(): supplied resource is not a valid stream resource
Done
