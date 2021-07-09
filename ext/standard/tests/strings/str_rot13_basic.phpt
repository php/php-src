--TEST--
Test str_rot13() function : basic functionality
--FILE--
<?php
echo "*** Testing str_rot13() : basic functionality ***\n";

echo "\nBasic tests\n";
var_dump(str_rot13("str_rot13() tests starting"));
var_dump(str_rot13("abcdefghijklmnopqrstuvwxyz"));

echo "\nEnsure numeric characters are left untouched\n";
if (strcmp(str_rot13("0123456789"), "0123456789") == 0) {
    echo "Strings equal : TEST PASSED\n";
} else {
    echo "Strings unequal : TEST FAILED\n";
}

echo "\nEnsure non-alphabetic characters are left untouched\n";
if (strcmp(str_rot13("!%^&*()_-+={}[]:;@~#<,>.?"), "!%^&*()_-+={}[]:;@~#<,>.?") == 0) {
    echo "Strings equal : TEST PASSED\n";
} else {
    echo "Strings unequal : TEST FAILED\n";
}

echo "\nEnsure strings round trip\n";
$str = "str_rot13() tests starting for Round Trip";
$encode = str_rot13($str);
$decode = str_rot13($encode);
if (strcmp($str, $decode) == 0) {
    echo "Strings equal : TEST PASSED\n";
} else {
    echo "Strings unequal : TEST FAILED\n";
}

echo "\nSIMD tests\n";
var_dump(str_rot13("str_rot13() tests starting str_rot13() tests starting"));
var_dump(str_rot13("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza"));
var_dump(str_rot13("01234567890123456789012345678901234567890123456789012"));
var_dump(str_rot13("!%^&*()_-+={}[]:;@~#<,>.?!%^&*()_-+={}[]:;@~#<,>.?!%^"));
var_dump(str_rot13("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZA"));
var_dump(str_rot13("abcdefghijklmnopqrstuvw123ABCDEFGHIJKL@~#<,>STUVWXYZA"));
var_dump(str_rot13("abcd12+=ABCD56@Z"));
var_dump(str_rot13("abcd12+=ABCD56@ZyY:"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@Z"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@ZyY:"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@Z"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@ZyY:"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@Z"));
var_dump(str_rot13("abcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@Zabcd12+=ABCD56@ZyY:"));

?>
--EXPECT--
*** Testing str_rot13() : basic functionality ***

Basic tests
string(26) "fge_ebg13() grfgf fgnegvat"
string(26) "nopqrstuvwxyzabcdefghijklm"

Ensure numeric characters are left untouched
Strings equal : TEST PASSED

Ensure non-alphabetic characters are left untouched
Strings equal : TEST PASSED

Ensure strings round trip
Strings equal : TEST PASSED

SIMD tests
string(53) "fge_ebg13() grfgf fgnegvat fge_ebg13() grfgf fgnegvat"
string(53) "nopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmn"
string(53) "01234567890123456789012345678901234567890123456789012"
string(53) "!%^&*()_-+={}[]:;@~#<,>.?!%^&*()_-+={}[]:;@~#<,>.?!%^"
string(53) "NOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN"
string(53) "nopqrstuvwxyzabcdefghij123NOPQRSTUVWXY@~#<,>FGHIJKLMN"
string(16) "nopq12+=NOPQ56@M"
string(19) "nopq12+=NOPQ56@MlL:"
string(32) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@M"
string(35) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@MlL:"
string(48) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@M"
string(51) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@MlL:"
string(64) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@M"
string(67) "nopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@Mnopq12+=NOPQ56@MlL:"
