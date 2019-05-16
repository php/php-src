--TEST--
Test chr() function : error conditions
--FILE--
<?php

/* Prototype  : string chr  ( int $ascii  )
 * Description: Return a specific character
 * Source code: ext/standard/string.c
*/

echo "*** Testing chr() : error conditions ***\n";

echo "\n-- Testing chr() function with no arguments --\n";
try {
    var_dump( chr() );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing chr() function with more than expected no. of arguments --\n";
$extra_arg = 10;
try {
    var_dump( chr(72, $extra_arg) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECTF--
*** Testing chr() : error conditions ***

-- Testing chr() function with no arguments --
Wrong parameter count for chr()

-- Testing chr() function with more than expected no. of arguments --
Wrong parameter count for chr()
===DONE===
