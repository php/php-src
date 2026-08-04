--TEST--
locale_is_right_to_left
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(Locale::isRightToLeft("en-US"));
var_dump(Locale::isRightToLeft("\INVALID\\"));
var_dump(Locale::isRightToLeft(""));
var_dump(Locale::isRightToLeft("ar"));
try {
	Locale::isRightToLeft("a\0r");
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
ValueError: Locale::isRightToLeft(): Argument #1 ($locale) must not contain any null bytes
