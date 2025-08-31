--TEST--
Bug #69180: Reflection does not honor trait conflict resolution / method aliasing
--FILE--
<?php

trait T1
{
    public function foo()
    {
    }
}

trait T2
{
    use T1 { foo as bar; }

    public function foo()
    {
    }
}


class C
{
    use T2;
}

$class = new ReflectionClass('C');

foreach ($class->getMethods() as $method) {
    var_dump($method->getName());
}

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
