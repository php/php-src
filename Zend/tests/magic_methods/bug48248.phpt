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
Fatal error: Uncaught Error: Cannot access private property B::$test in %s:%d
Stack trace:
#0 %s(%d): A->__get('test')
#1 {main}
  thrown in %s on line %d
