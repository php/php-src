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
echo "-- Testing ftell with closed/unset file handle --\n";
$fp = fopen(__FILE__, "r");
fclose($fp);
try {
    var_dump(ftell($fp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing ftell() : error conditions ***
-- Testing ftell with closed/unset file handle --
ftell(): supplied resource is not a valid stream resource
Done
