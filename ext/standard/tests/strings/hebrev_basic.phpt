--TEST--
Test hebrev() function : basic functionality
--FILE--
<?php

/* Prototype  : string hebrev  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrev() : basic functionality ***\n";

$hebrew_text = "The hebrev function converts logical Hebrew text to visual text.\nThe function tries to avoid breaking words.\n";

var_dump(hebrev($hebrew_text));
var_dump(hebrev($hebrew_text, 15));

?>
===DONE===
--EXPECT--
*** Testing hebrev() : basic functionality ***
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
string(109) "to visual text
Hebrew text
logical
converts
hebrev function
.The
breaking words
tries to avoid
.The function
"
===DONE===
