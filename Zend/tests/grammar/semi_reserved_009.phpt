--TEST--
insteadof insteadof <>, as as <>
--FILE--
<?php

trait TraitA
{
    public static function as(){ echo __METHOD__, PHP_EOL; }
    public static function insteadof(){ echo __METHOD__, PHP_EOL; }
}

trait TraitB
{
    public static function insteadof(){ echo __METHOD__, PHP_EOL; }
}

class Foo
{
    use TraitA , TraitB {
        as as alias;
        TraitB::insteadof insteadof TraitA;
    }
}

Foo::alias();

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
TraitA::as

Done
