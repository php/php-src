--TEST--
Attributes make use of correct scope.
--FILE--
<?php

<<A1(self::class, self::FOO)>>
class C1
{
	<<A1(self::class, self::FOO)>>
	private const FOO = 'foo';
	
	<<A1(self::class, self::FOO)>>
	public $a;
	
	<<A1(self::class, self::FOO)>>
	public function bar(<<A1(self::class, self::FOO)>> $p) { }
}

$ref = new \ReflectionClass(C1::class);
print_r($ref->getAttributes()[0]->getArguments());
print_r($ref->getReflectionConstant('FOO')->getAttributes()[0]->getArguments());
print_r($ref->getProperty('a')->getAttributes()[0]->getArguments());
print_r($ref->getMethod('bar')->getAttributes()[0]->getArguments());
print_r($ref->getMethod('bar')->getParameters()[0]->getAttributes()[0]->getArguments());

echo "\n";

class C2
{
	private const FOO = 'foo';
	
	public static function foo()
	{
		return <<A1(self::class, self::FOO)>> function (<<A1(self::class, self::FOO)>> $p) { };
	}
}

$ref = new \ReflectionFunction(C2::foo());
print_r($ref->getAttributes()[0]->getArguments());
print_r($ref->getParameters()[0]->getAttributes()[0]->getArguments());

echo "\n";

class C3
{
	private const FOO = 'foo';
	
	public static function foo()
	{
		return new <<A1(self::class, self::FOO)>> class() {
			private const FOO = 'bar';
			
			<<A1(self::class, self::FOO)>>
			public function bar() { }
		};
	}
}

$obj = C3::foo();
$ref = new \ReflectionObject($obj);
$name = $ref->getMethod('bar')->getAttributes()[0]->getArguments()[0];

print_r($ref->getAttributes()[0]->getArguments());
var_dump($name == get_class($obj));
var_dump($ref->getMethod('bar')->getAttributes()[0]->getArguments()[1]);

?>
--EXPECT--
Array
(
    [0] => C1
    [1] => foo
)
Array
(
    [0] => C1
    [1] => foo
)
Array
(
    [0] => C1
    [1] => foo
)
Array
(
    [0] => C1
    [1] => foo
)
Array
(
    [0] => C1
    [1] => foo
)

Array
(
    [0] => C2
    [1] => foo
)
Array
(
    [0] => C2
    [1] => foo
)

Array
(
    [0] => C3
    [1] => foo
)
bool(true)
string(3) "bar"
