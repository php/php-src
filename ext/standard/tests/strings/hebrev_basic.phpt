--TEST--
Test hebrev() function : basic functionality
--FILE--
<?php

echo "*** Testing hebrev() : basic functionality ***\n";

$hebrew_text = "The hebrev function converts logical Hebrew text to visual text.\nThe function tries to avoid breaking words.\n";

var_dump(hebrev($hebrew_text));
var_dump(hebrev($hebrew_text, 15));

?>
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
