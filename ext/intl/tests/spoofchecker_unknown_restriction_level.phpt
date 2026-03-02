--TEST--
Spoofchecker attempting to pass an unknown restriction level
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists("Spoofchecker")) print 'skip';

if (!method_exists(new Spoofchecker(), 'setRestrictionLevel')) print 'skip ICU version < 58';
?>
--FILE--
<?php

$x = new Spoofchecker();
try {
    $x->setRestrictionLevel(Spoofchecker::SINGLE_SCRIPT);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
Spoofchecker::setRestrictionLevel(): Argument #1 ($level) must be one of Spoofchecker::ASCII, Spoofchecker::SINGLE_SCRIPT_RESTRICTIVE, Spoofchecker::HIGHLY_RESTRICTIVE, Spoofchecker::MODERATELY_RESTRICTIVE, Spoofchecker::MINIMALLY_RESTRICTIVE, or Spoofchecker::UNRESTRICTIVE
