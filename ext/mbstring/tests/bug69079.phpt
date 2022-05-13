--TEST--
Bug #69079 (enhancement for mb_substitute_character)
--EXTENSIONS--
mbstring
--FILE--
<?php

mb_internal_encoding('UTF-8');
var_dump(mb_substitute_character(0x1F600));
var_dump(bin2hex(mb_scrub("\xff")));
mb_substitute_character(0x3f); // Reset to '?', as the next call will fail
try {
    var_dump(mb_substitute_character(0xD800)); // Surrogate (illegal)
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(bin2hex(mb_scrub("\xff")));

mb_internal_encoding('EUC-JP-2004');

mb_substitute_character(0x63); // Reset to '?', as the next call will fail
try {
    mb_substitute_character(0x8fa1ef); // EUC-JP-2004 encoding of U+50AA (illegal)
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(bin2hex(mb_scrub("\x8d")));

mb_substitute_character(0x50aa);
var_dump(bin2hex(mb_scrub("\x8d")));

?>
--EXPECT--
bool(true)
string(8) "f09f9880"
mb_substitute_character(): Argument #1 ($substitute_character) is not a valid codepoint
string(2) "3f"
mb_substitute_character(): Argument #1 ($substitute_character) is not a valid codepoint
string(2) "63"
string(6) "8fa1ef"
