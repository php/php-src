--TEST--
Transliterator::transliterate (error)
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=true
--FILE--
<?php

$tr = Transliterator::create("latin");

try {
	var_dump(transliterator_transliterate($tr, "str", 7));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    transliterator_transliterate($tr, "str", 7, 6);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

//bad UTF-8
try {
	transliterator_transliterate($tr, "\x80\x03");
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlException: transliterator_transliterate(): Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3)
ValueError: transliterator_transliterate(): Argument #2 ($string) must be less than or equal to argument #3 ($end)
IntlException: transliterator_transliterate(): String conversion of string to UTF-16 failed
