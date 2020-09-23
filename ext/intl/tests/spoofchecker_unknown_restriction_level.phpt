--TEST--
Spoofchecker attempting to pass an unknown restriction level
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip'; ?>
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
Spoofchecker::setRestrictionLevel(): Argument #1 ($level) must be one of the Spoofchecker::*_RESTRICTIVE constants or Spoofchecker::UNRESTRICTIVE
