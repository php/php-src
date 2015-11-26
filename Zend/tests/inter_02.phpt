--TEST--
Namespace constant as value default
--FILE--
<?php

namespace foo;

error_reporting(E_ALL);

interface foo {
    const foo = 2;
}

function foo($x = \foo\foo::foo) {
    var_dump($x);
}

foo();
?>
--EXPECT--
int(2)
