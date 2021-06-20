--TEST--
NamedParameterAlias Attribute
--FILE--
<?php

function test(#[NamedParameterAlias(1234)] $foo) {
    echo $foo;
}
--EXPECTF--
Fatal error: NamedParameterAlias::__construct(): Argument #1 ($alias) must be of type string, int given in %s
