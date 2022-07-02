--TEST--
Test mb_substitute_character() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_substitute_character() : basic functionality ***\n";


// Initialise all required variables
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character(66) );
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character(1234) );
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character('none') );
var_dump( mb_substitute_character() );
// Check string case insensitivity
var_dump( mb_substitute_character('LoNg') );
var_dump( mb_substitute_character() );
try {
    var_dump( mb_substitute_character("b") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_substitute_character() : basic functionality ***
int(63)
bool(true)
int(66)
bool(true)
int(1234)
bool(true)
string(4) "none"
bool(true)
string(4) "long"
mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
