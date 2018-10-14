--TEST--
Bug #47254
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
class A
{
	protected function a() {}

}

class B extends A
{
	public function b() {}
}

$B = new B();
$R = new ReflectionObject($B);
$m = $R->getMethods();
print_r($m);

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; A has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; B has a deprecated constructor in %s on line %d
Array
(
    [0] => ReflectionMethod Object
        (
            [name] => b
            [class] => B
        )

    [1] => ReflectionMethod Object
        (
            [name] => a
            [class] => A
        )

)
