--TEST--
Edge case: T_STRING<insteadof> insteadof T_STRING<?>
--FILE--
<?php

trait TraitA
{
    public static function insteadof(){ echo __METHOD__, PHP_EOL; }
}

trait TraitB
{
    public static function insteadof(){ echo __METHOD__, PHP_EOL; }
}

class Foo
{
    use TraitA , TraitB {
        TraitB::insteadof
            insteadof TraitA;
    }
}

Foo::insteadof();

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
TraitB::insteadof

Done
