--TEST--
transliterator_transliterate (variant 1, non-transliterator 1st arg)
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=true
--FILE--
<?php

$str = " o";

echo transliterator_transliterate("[\p{White_Space}] hex", $str), "\n";

try {
	echo transliterator_transliterate("\x8F", $str), "\n";
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

class A {
	function __toString() { return "inexistent id"; }
}

try {
	echo transliterator_transliterate(new A(), $str), "\n";
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
\u0020o
IntlException: transliterator_transliterate(): String conversion of id to UTF-16 failed
IntlException: transliterator_transliterate(): unable to open ICU transliterator with id "inexistent id"
