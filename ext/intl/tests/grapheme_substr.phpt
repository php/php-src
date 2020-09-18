--TEST--
Test grapheme_substr() function
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

var_dump(grapheme_substr("", 0, 5));

try {
    grapheme_substr("", 1, 5);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(grapheme_substr("abc", 0, 5));

try {
    grapheme_substr("abc", 3, 5);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(0) ""
grapheme_substr(): Argument #2 ($start) must be contained in argument #1 ($string)
string(3) "abc"
grapheme_substr(): Argument #2 ($start) must be contained in argument #1 ($string)
