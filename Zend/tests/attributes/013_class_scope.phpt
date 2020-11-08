--TEST--
Attributes make use of class scope.
--FILE--
<?php

#[A1(self::class, self::FOO)]
class C1
{
    #[A1(self::class, self::FOO)]
    private const FOO = 'foo';

    #[A1(self::class, self::FOO)]
    public $a;

    #[A1(self::class, self::FOO)]
    public function bar(#[A1(self::class, self::FOO)] $p) { }
}

$ref = new \ReflectionClass(C1::class);
print_r($ref->getAttributes()[0]->getArguments());
print_r($ref->getReflectionConstant('FOO')->getAttributes()[0]->getArguments());
print_r($ref->getProperty('a')->getAttributes()[0]->getArguments());
print_r($ref->getMethod('bar')->getAttributes()[0]->getArguments());
print_r($ref->getMethod('bar')->getParameters()[0]->getAttributes()[0]->getArguments());

echo "\n";

trait T1
{
    #[A1(self::class, self::FOO)]
    public function foo() { }
}

class C2
{
    use T1;

    private const FOO = 'bar';
}

$ref = new \ReflectionClass(C2::class);
print_r($ref->getMethod('foo')->getAttributes()[0]->getArguments());

$ref = new \ReflectionClass(T1::class);
$attr = $ref->getMethod('foo')->getAttributes()[0];

try {
    $attr->getArguments();
} catch (\Error $e) {
    var_dump('ERROR 1', $e->getMessage());
}

echo "\n";

class C3
{
    private const FOO = 'foo';

    public static function foo()
    {
        return new #[A1(self::class, self::FOO)] class() {
            private const FOO = 'bar';

            #[A1(self::class, self::FOO)]
            public function bar() { }
        };
    }
}

$ref = new \ReflectionObject(C3::foo());

$args = $ref->getAttributes()[0]->getArguments();
var_dump($args[0] == $ref->getName(), $args[1]);

$args = $ref->getMethod('bar')->getAttributes()[0]->getArguments();
var_dump($args[0] == $ref->getName(), $args[1]);

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
    [1] => bar
)
string(7) "ERROR 1"
string(28) "Undefined constant self::FOO"

bool(true)
string(3) "bar"
bool(true)
string(3) "bar"
