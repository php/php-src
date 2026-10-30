--TEST--
Transliterator::transliterate (error)
--EXTENSIONS--
intl
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$tr = Transliterator::create("latin");

function dump_value_error(callable $callback): void {
    try {
        $callback();
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

//Arguments
var_dump(transliterator_transliterate($tr,"str",7));

dump_value_error(function() use ($tr) {
    transliterator_transliterate($tr,"str",7,6);
});

dump_value_error(function() use ($tr) {
    transliterator_transliterate($tr,"str", 0, -2);
});

dump_value_error(function() use ($tr) {
    transliterator_transliterate($tr,"str", -1);
});

dump_value_error(function() {
    transliterator_transliterate("latin","str", -1);
});

dump_value_error(function() use ($tr) {
    $tr->transliterate("str", 7, 6);
});

//bad UTF-8
transliterator_transliterate($tr, "\x80\x03");

echo "Done.\n";
?>
--EXPECTF--
Warning: transliterator_transliterate(): transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3) in %s on line %d
bool(false)
transliterator_transliterate(): Argument #3 ($start) must be less than or equal to argument #4 ($end)
transliterator_transliterate(): Argument #4 ($end) must be greater than or equal to -1
transliterator_transliterate(): Argument #3 ($start) must be greater than or equal to 0
transliterator_transliterate(): Argument #3 ($start) must be greater than or equal to 0
Transliterator::transliterate(): Argument #2 ($start) must be less than or equal to argument #3 ($end)

Warning: transliterator_transliterate(): String conversion of string to UTF-16 failed in %s on line %d
Done.
