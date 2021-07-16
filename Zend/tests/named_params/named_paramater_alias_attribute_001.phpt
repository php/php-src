--TEST--
NamedParameterAlias Attribute
--FILE--
<?php

function test(#[NamedParameterAlias("bar")] $foo) {
    echo $foo;
}

test(bar: "Hello World");
--EXPECT--
Hello World
