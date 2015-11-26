--TEST--
Checking trait_exists(): interface, trait, abstract and final class
--FILE--
<?php

interface a { }

abstract class b { }

final class c { }

trait d {}

var_dump(trait_exists('a'));
var_dump(trait_exists('b'));
var_dump(trait_exists('c'));
var_dump(trait_exists('d'));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
