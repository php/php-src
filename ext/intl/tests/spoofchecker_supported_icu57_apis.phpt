--TEST--
Spoofchecker exposes restriction-level APIs on all supported ICU versions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (!class_exists("Spoofchecker")) print 'skip'; ?>
<?php
    $r = new ReflectionClass("Spoofchecker");
    if (false === $r->getConstant("SINGLE_SCRIPT_RESTRICTIVE")) {
        die("skip Incompatible ICU version");
    }
?>
--FILE--
<?php
$r = new ReflectionClass("Spoofchecker");

$ascii = $r->getConstant("ASCII");
$singleScriptRestrictive = $r->getConstant("SINGLE_SCRIPT_RESTRICTIVE");
$mixedNumbers = $r->getConstant("MIXED_NUMBERS");

var_dump($ascii !== false);
var_dump($singleScriptRestrictive !== false);
var_dump($mixedNumbers !== false);
var_dump(is_int($ascii));
var_dump(is_int($singleScriptRestrictive));
var_dump(is_int($mixedNumbers));
var_dump($ascii !== $singleScriptRestrictive);
var_dump($ascii !== $mixedNumbers);
var_dump($singleScriptRestrictive !== $mixedNumbers);
var_dump($r->hasMethod("setRestrictionLevel"));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
