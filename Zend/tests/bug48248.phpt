--TEST--
Bug #48248 (SIGSEGV when access to private property via &__get)
--FILE--
<?php

class A
{
    public function & __get($name)
    {
        return $this->test;
    }
}

class B extends A
{
    private $test;
}

$b = new B;
var_dump($b->test);

?>
--EXPECTF--
Notice: Undefined property: B::$test in %s on line %d

Notice: Only variable references should be returned by reference in %s on line %d
NULL
