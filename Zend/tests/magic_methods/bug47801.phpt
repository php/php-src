--TEST--
Bug #47801 (__call() accessed via parent:: operator is provided incorrect method name)
--FILE--
<?php

class A
{
  function __call($name, $args)
  {
    echo("magic method called: $name\n");
  }
}

class B extends A
{
  function getFoo()
  {
    parent::getFoo();
  }
}

$a = new A();
$a->getFoo();

$b = new B();
$b->getFoo();

?>
--EXPECT--
magic method called: getFoo
magic method called: getFoo
