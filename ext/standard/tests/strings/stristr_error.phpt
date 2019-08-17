--TEST--
Test stristr() function : error conditions
--FILE--
<?php

/* Prototype: string stristr  ( string $haystack  , mixed $needle  [, bool $before_needle  ] )
   Description: Case-insensitive strstr()
*/
echo "*** Testing stristr() : error conditions ***\n";

echo "\n-- Testing stristr() function with empty haystack --\n";
try {
    stristr(NULL, "");
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing stristr() function with empty needle --\n";
try {
    stristr("Hello World", "");
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECTF--
*** Testing stristr() : error conditions ***

-- Testing stristr() function with empty haystack --
Empty needle

-- Testing stristr() function with empty needle --
Empty needle
===DONE===
