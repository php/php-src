--TEST--
Test convert_cyr_string() function : basic functionality 
--FILE--
<?php

/* Prototype  : string convert_cyr_string  ( string $str  , string $from  , string $to  )
 * Description: Convert from one Cyrillic character set to another
 * Source code: ext/standard/string.c
*/

echo "*** Testing convert_cyr_string() : basic functionality ***\n";

$str = b"Convert from one Cyrillic character set to another.";

echo "\n-- First try some simple English text --\n";
var_dump(bin2hex(convert_cyr_string($str, 'w', 'k')));
var_dump(bin2hex(convert_cyr_string($str, 'w', 'i')));


echo "\n-- Now try some of characters in 128-255 range --\n";

$i = 128;
echo "$i: \n"; 
echo bin2hex(convert_cyr_string(chr($i), 'w', 'k')) . "\n";
$i = 200;
echo "$i: \n"; 
echo bin2hex(convert_cyr_string(chr($i), 'w', 'k')) . "\n";
$i = 255;
echo "$i: \n"; 
echo bin2hex(convert_cyr_string(chr($i), 'w', 'k')) . "\n";

?>
===DONE===
--EXPECTF--
*** Testing convert_cyr_string() : basic functionality ***

-- First try some simple English text --
unicode(102) "436f6e766572742066726f6d206f6e6520437972696c6c6963206368617261637465722073657420746f20616e6f746865722e"
unicode(102) "436f6e766572742066726f6d206f6e6520437972696c6c6963206368617261637465722073657420746f20616e6f746865722e"

-- Now try some of characters in 128-255 range --
128: 

Warning: convert_cyr_string() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d

200: 

Warning: convert_cyr_string() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d

255: 

Warning: convert_cyr_string() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d

===DONE===
