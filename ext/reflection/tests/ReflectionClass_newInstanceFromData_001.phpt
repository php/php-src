--TEST--
ReflectionClass::newInstanceFromData
--FILE--
<?php

class A
{
    public int $a;
    public string $b;

    public function __construct($c, $d)
    {
        echo "In constructor of class A\n";
    }
}

class B
{
    public int $a;
    public readonly string $b;
}

class C
{
}

#[\AllowDynamicProperties]
class D
{
}

class E
{
    public function __construct(
        public int $a,
        public string $b,
    )
    {}
}

class F
{
    public readonly int $a;

    public function __construct(
        public readonly string $b,
    )
    {}
}

class G
{
    public readonly int $a;
    public readonly string $b;

    public function __construct()
    {
        $this->b = 456;
    }
}

class H
{
    public int $a {
        set(int $value) => $value + 1;
    }
}

class I
{
    public int $a;

    public int $b {
        get => $this->a + 1;
        set(int $value) {
            $this->a = $value - 1;
        }
    }
}


$rcA = new ReflectionClass('A');
$rcB = new ReflectionClass('B');
$rcC = new ReflectionClass('C');
$rcD = new ReflectionClass('D');
$rcE = new ReflectionClass('E');
$rcF = new ReflectionClass('F');
$rcG = new ReflectionClass('G');
$rcH = new ReflectionClass('H');
$rcI = new ReflectionClass('I');

// assign bad data type to normal class
try
{
    $rcA->newInstanceFromData(['a' => 'bad', 'b' => 123], ['foo', 1337]);
    echo "you should not see this\n";
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

// normal class with constructor
var_dump($rcA->newInstanceFromData(['a' => 123, 'b' => 'good'], ['foo', 1337]));

// normal class with no constructor and a readonly property
var_dump($rcB->newInstanceFromData(['a' => 123, 'b' => 'good']));

// trying to set dynamic properties on class without AllowDynamicProperties attribute
var_dump($rcC->newInstanceFromData(['a' => 123, 'b' => 'good'])); // this should warn
var_dump($rcC->newInstanceFromData([])); // this is fine

// setting dynamic properties on a class with AllowDynamicProperties attribute
var_dump($rcD->newInstanceFromData(['a' => 123, 'b' => 'good']));

// class with property promotion
try
{
    $rcE->newInstanceFromData(['a' => 123, 'b' => 'good']); // no constructor args will fail
    echo "you should not see this\n";
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

var_dump($rcE->newInstanceFromData(['a' => 123, 'b' => 'good'], [456, 'foo'])); // constructor args will override class props

// class with readonly promoted property
var_dump($rcF->newInstanceFromData(['a' => 123], ['b' => 'good']));

try
{
    var_dump($rcF->newInstanceFromData(['a' => 123, 'b' => 'first'], ['b' => 'second']));
    echo "you should not see this\n";
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

// readonly property set in the constructor
try
{
    $rcG->newInstanceFromData(['a' => 123, 'b' => 'good']); // setting $b by data will conflict with constructor's set
    echo "you should not see this\n";
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

// hooked set property
var_dump($rcH->newInstanceFromData(['a' => 1]));

// virtual property
var_dump($rcI->newInstanceFromData(['a' => 1, 'b' => 2]));

$instance = $rcI->newInstanceFromData(['a' => 1]);
var_dump($instance);
var_dump($instance->b);
$instance->b = 3;
var_dump($instance->b);

?>
--EXPECTF--
Exception: Cannot assign string to property A::$a of type int
In constructor of class A
object(A)#%d (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
object(B)#%d (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}

Deprecated: Creation of dynamic property C::$a is deprecated in %s on line %d

Deprecated: Creation of dynamic property C::$b is deprecated in %s on line %d
object(C)#%d (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
object(C)#%d (0) {
}
object(D)#%d (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
Exception: Too few arguments to function E::__construct(), 0 passed and exactly 2 expected
object(E)#%d (2) {
  ["a"]=>
  int(456)
  ["b"]=>
  string(3) "foo"
}
object(F)#%d (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
Exception: Cannot modify readonly property F::$b
Exception: Cannot modify readonly property G::$b
object(H)#%d (1) {
  ["a"]=>
  int(2)
}
object(I)#%d (1) {
  ["a"]=>
  int(1)
}
object(I)#%d (1) {
  ["a"]=>
  int(1)
}
int(2)
int(3)
