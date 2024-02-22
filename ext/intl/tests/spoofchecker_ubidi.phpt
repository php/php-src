--TEST--
spoofchecker checks if strings are confusable in a given direction.
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '74.0') < 0)die('skip for ICU >= 74'); ?>
--FILE--
<?php
$s = new SpoofChecker();
try {
    $s->areBidiConfusable(SpoofChecker::UBIDI_RTL + 1, "a", "a");
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}
var_dump($s->areBidiConfusable(SpoofChecker::UBIDI_LTR, "\x73\x63", "sc"));
var_dump($s->areBidiConfusable(SpoofChecker::UBIDI_RTL, "sc", "\x73\x63"));
--EXPECT--
Spoofchecker::areBidiConfusable(): Argument #1 ($direction) must be either SpoofChecker::UBIDI_LTR or SpoofChecker::UBIDI_RTL
bool(true)
bool(true)
