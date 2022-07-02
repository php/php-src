--TEST--
mb_substitute_character()
--EXTENSIONS--
mbstring
--FILE--
<?php

// Note: It does not return TRUE/FALSE for setting char

var_dump(mb_substitute_character(0x3044));
var_dump(mb_substitute_character());
var_dump(bin2hex(mb_convert_encoding("\xe2\x99\xa0\xe3\x81\x82", "CP932", "UTF-8")));

var_dump(mb_substitute_character('long'));
var_dump(mb_substitute_character());
var_dump(bin2hex(mb_convert_encoding("\xe2\x99\xa0\xe3\x81\x82", "CP932", "UTF-8")));

var_dump(mb_substitute_character('none'));
var_dump(mb_substitute_character());
var_dump(bin2hex(mb_convert_encoding("\xe2\x99\xa0\xe3\x81\x82", "CP932", "UTF-8")));

var_dump(mb_substitute_character('entity'));
var_dump(mb_substitute_character());
var_dump(bin2hex(mb_convert_encoding("\xe2\x99\xa0\xe3\x81\x82", "CP932", "UTF-8")));

try {
    var_dump(mb_substitute_character('BAD_NAME'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bool(true)
int(12356)
string(8) "82a282a0"
bool(true)
string(4) "long"
string(16) "552b3236363082a0"
bool(true)
string(4) "none"
string(4) "82a0"
bool(true)
string(6) "entity"
string(20) "262378323636303b82a0"
mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
