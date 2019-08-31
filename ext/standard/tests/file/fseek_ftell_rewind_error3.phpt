--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - rewind()
--FILE--
<?php

/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

echo "*** Testing rewind() : error conditions ***\n";

// rewind on a file handle which is already closed
echo "-- Testing rewind() with closed/unset file handle --\n";
$fp = fopen(__FILE__, "r");
fclose($fp);
try {
    var_dump(rewind($fp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing rewind() : error conditions ***
-- Testing rewind() with closed/unset file handle --
rewind(): supplied resource is not a valid stream resource
Done
