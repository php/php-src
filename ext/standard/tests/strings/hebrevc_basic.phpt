--TEST--
Test hebrevc() function : basic functionality
--FILE--
<?php

/* Prototype  : string hebrevc  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrevc() : basic functionality ***\n";

$hebrew_text = "The hebrevc function converts logical Hebrew text to visual text.\nThis function is similar to hebrev() with the difference that it converts newlines (\n) to '<br>\n'.\nThe function tries to avoid breaking words.\n";

var_dump(hebrevc($hebrew_text));
var_dump(hebrevc($hebrew_text, 15));

?>
===DONE===
--EXPECTF--
*** Testing hebrevc() : basic functionality ***
string(239) ".The hebrevc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrev() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
string(317) "to visual text<br />
Hebrew text<br />
logical<br />
converts<br />
function<br />
.The hebrevc<br />
newlines<br />
it converts<br />
difference that<br />
with the<br />
to hebrev()<br />
is similar<br />
) This function<br />
<to '<br (<br />
.'<br />
breaking words<br />
tries to avoid<br />
.The function<br />
"
===DONE===
