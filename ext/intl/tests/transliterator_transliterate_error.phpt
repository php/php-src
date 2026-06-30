--TEST--
Transliterator::transliterate (error)
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=true
--FILE--
<?php

$tr = Transliterator::create("latin");

function dump_throwable(callable $callback): void {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

//Arguments
dump_throwable(function() use ($tr) {
    var_dump(transliterator_transliterate($tr, "str", 7));
});

dump_throwable(function() use ($tr) {
    transliterator_transliterate($tr, "str", 7, 6);
});

dump_throwable(function() use ($tr) {
    transliterator_transliterate($tr, "str", 0, -2);
});

dump_throwable(function() use ($tr) {
    transliterator_transliterate($tr, "str", -1);
});

dump_throwable(function() {
    transliterator_transliterate("latin", "str", -1);
});

dump_throwable(function() use ($tr) {
    $tr->transliterate("str", 7, 6);
});

//bad UTF-8
try {
	transliterator_transliterate($tr, "\x80\x03");
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlException: transliterator_transliterate(): Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3)
ValueError: transliterator_transliterate(): Argument #3 ($start) must be less than or equal to argument #4 ($end)
ValueError: transliterator_transliterate(): Argument #4 ($end) must be greater than or equal to -1
ValueError: transliterator_transliterate(): Argument #3 ($start) must be greater than or equal to 0
ValueError: transliterator_transliterate(): Argument #3 ($start) must be greater than or equal to 0
ValueError: Transliterator::transliterate(): Argument #2 ($start) must be less than or equal to argument #3 ($end)
IntlException: transliterator_transliterate(): String conversion of string to UTF-16 failed
