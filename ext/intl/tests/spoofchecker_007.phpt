--TEST--
spoofchecker with restriction level
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip'; ?>
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
