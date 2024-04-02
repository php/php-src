--TEST--
Test join() function: error conditions
--FILE--
<?php
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
join(): If argument #1 ($separator) is of type string, argument #2 ($array) must be of type array, null given
Done
