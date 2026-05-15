--TEST--
Spoofchecker exposes restriction-level APIs on all supported ICU versions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (!class_exists("Spoofchecker")) print 'skip'; ?>
--FILE--
<?php
$r = new ReflectionClass("Spoofchecker");

var_dump($r->getConstant("ASCII") !== false);
var_dump($r->getConstant("SINGLE_SCRIPT_RESTRICTIVE") !== false);
var_dump($r->getConstant("MIXED_NUMBERS") !== false);
var_dump($r->hasMethod("setRestrictionLevel"));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
