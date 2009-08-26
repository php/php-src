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
--CLEAN--
--EXPECT--
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
