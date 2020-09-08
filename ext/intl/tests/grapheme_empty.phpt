--TEST--
Test grapheme_strpos-alike functions with empty needle
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

try {
    var_dump(grapheme_strpos("abc", "", -1));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strpos("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strpos("abc", "", -1));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_stripos("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_stripos("abc", "", -1));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strrpos("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strrpos("abc", "", -1));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strripos("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strripos("abc", "", 1));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_strstr("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(grapheme_stristr("abc", ""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
grapheme_strpos(): Argument #2 ($needle) cannot be empty
grapheme_strpos(): Argument #2 ($needle) cannot be empty
grapheme_strpos(): Argument #2 ($needle) cannot be empty
grapheme_stripos(): Argument #2 ($needle) cannot be empty
grapheme_stripos(): Argument #2 ($needle) cannot be empty
grapheme_strrpos(): Argument #2 ($needle) cannot be empty
grapheme_strrpos(): Argument #2 ($needle) cannot be empty
grapheme_strripos(): Argument #2 ($needle) cannot be empty
grapheme_strripos(): Argument #2 ($needle) cannot be empty
grapheme_strstr(): Argument #2 ($needle) cannot be empty
grapheme_stristr(): Argument #2 ($needle) cannot be empty
