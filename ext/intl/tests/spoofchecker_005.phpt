--TEST--
spoofchecker with settings changed
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker") || version_compare(INTL_ICU_VERSION, '51') < 0) print 'skip'; ?>
--FILE--
<?php
$x = new Spoofchecker;
$x->setChecks(Spoofchecker::MODERATELY_RESTRICTIVE);
var_dump(
    $x->isSuspicious('ёxample.com')
);
?>
--EXPECTF--
bool(false)
