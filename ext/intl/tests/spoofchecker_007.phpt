--TEST--
spoofchecker with restriction level
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
<?php
    $r = new ReflectionClass("SpoofChecker");
    if (false === $r->getConstant("SINGLE_SCRIPT_RESTRICTIVE")) {
        die("skip Incompatible ICU version");
    }
?>
--FILE--
<?php

$x = new Spoofchecker();
$x->setRestrictionLevel(Spoofchecker::HIGHLY_RESTRICTIVE);

$mixed = "\u{91CE}\u{7403}\u{30FC}";
var_dump($x->isSuspicious($mixed));

$x->setRestrictionLevel(Spoofchecker::SINGLE_SCRIPT_RESTRICTIVE);

$mixed = "\u{91CE}\u{7403}\u{30FC} abc";
var_dump($x->isSuspicious($mixed));
?>
--EXPECT--
bool(false)
bool(true)
