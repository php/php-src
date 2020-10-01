--TEST--
Spoofchecker attempting to pass an unknown restriction level
--SKIPIF--
<?php
if (!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip';

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
Spoofchecker::setRestrictionLevel(): Argument #1 ($level) must be one of Spoofchecker::ASCII, Spoofchecker::SINGLE_SCRIPT_RESTRICTIVE, Spoofchecker::SINGLE_HIGHLY_RESTRICTIVE, Spoofchecker::SINGLE_MODERATELY_RESTRICTIVE, Spoofchecker::SINGLE_MINIMALLY_RESTRICTIVE, or Spoofchecker::UNRESTRICTIVE
