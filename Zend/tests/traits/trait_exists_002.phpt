--TEST--
Testing trait_exists() inside a namespace
--FILE--
<?php

namespace foo;

trait IFoo { }

trait ITest { }


var_dump(trait_exists('IFoo'));
var_dump(trait_exists('foo\\IFoo'));
var_dump(trait_exists('FOO\\ITEST'));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
