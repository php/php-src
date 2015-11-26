--TEST--
Testing interface_exists() inside a namespace
--FILE--
<?php

namespace foo;

interface IFoo { }

interface ITest extends IFoo { }

interface IBar extends IFoo { }


var_dump(interface_exists('IFoo'));
var_dump(interface_exists('foo\\IFoo'));
var_dump(interface_exists('FOO\\ITEST'));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
