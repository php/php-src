--TEST--
Test join() function: error conditions
--FILE--
<?php
/* Prototype  : string join( string $glue, array $pieces )
 * Description: Join array elements with a string
 * Source code: ext/standard/string.c
 * Alias of function: implode()
*/

echo "*** Testing join() : error conditions ***\n";

// Less than expected number of arguments
echo "\n-- Testing join() with less than expected no. of arguments --\n";
$glue = 'string_val';

try {
    var_dump(join($glue));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing join() : error conditions ***

-- Testing join() with less than expected no. of arguments --
Argument must be an array
Done
