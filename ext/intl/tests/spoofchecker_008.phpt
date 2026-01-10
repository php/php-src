--TEST--
spoofchecker with locale settings
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
--FILE--
<?php

$s = new Spoofchecker();

$s->setAllowedChars('[a-z]');
var_dump($s->isSuspicious("123"));
$s->setAllowedChars('[1-3]');
var_dump($s->isSuspicious("123"));
$s->setAllowedChars('[a-z]', SpoofChecker::IGNORE_SPACE | SpoofChecker::CASE_INSENSITIVE);
var_dump($s->isSuspicious("ABC"));

try {
	$s->setAllowedChars('[a-z]', 1024);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	$s->setAllowedChars("A-Z]");
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	$s->setAllowedChars("[A-Z");
} catch (\ValueError $e) {
	echo $e->getMessage();
}

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
Spoofchecker::setAllowedChars(): Argument #2 ($patternOptions) must be a valid pattern option, 0 or (SpoofChecker::IGNORE_SPACE|(<none> or SpoofChecker::USET_CASE_INSENSITIVE%s))
Spoofchecker::setAllowedChars(): Argument #1 ($pattern) must be a valid regular expression character set pattern
Spoofchecker::setAllowedChars(): Argument #1 ($pattern) must be a valid regular expression character set pattern
