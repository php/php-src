--TEST--
Bug #49354 (mb_strcut() cuts wrong length when offset is in the middle of a multibyte character)
--EXTENSIONS--
mbstring
--FILE--
<?php
$crap = 'AåBäCöDü'; // sequence of 1-byte and 2-byte chars: [41 c3a5 42 c3a4 43 c3b6 44 c3bc]
var_dump(mb_strcut($crap, 0, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 1, 100, 'UTF-8')); // skip over A
var_dump(mb_strcut($crap, 2, 100, 'UTF-8')); // cut in middle of å
var_dump(mb_strcut($crap, 3, 100, 'UTF-8')); // skip over Aå
var_dump(mb_strcut($crap, 12, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 13, 100, 'UTF-8'));

?>
--EXPECT--
string(12) "AåBäCöDü"
string(11) "åBäCöDü"
string(11) "åBäCöDü"
string(9) "BäCöDü"
string(0) ""
string(0) ""
