--TEST--
ReflectionParameter::get/hasType and ReflectionType tests
--FILE--
<?php
function foo(stdClass $a, array $b, callable $c, string $f, bool $g, int $h, float $i, NotExisting $j, stdClass $d = null, $e = null) { }

function bar(): stdClass { return new stdClass; }

class c extends stdClass {
  function bar(self $x): int { return 1; }
  function pbar(parent $x): int { return 1; }
  function factory(): self { return new c; }
  function pfactory(): parent { return new stdClass; }
}

$closure = function (Test $a): Test { return $a; };

echo "*** functions\n";

foreach ([
  new ReflectionFunction('foo'),
  new ReflectionFunction($closure),
] as $idx => $rf) {
  foreach ($rf->getParameters() as $idx2 => $rp) {
    echo "** Function $idx - Parameter $idx2\n";
    var_dump($rp->hasType());
    $ra = $rp->getType();
    if ($ra) {
      var_dump($ra->allowsNull());
      var_dump($ra->isBuiltin());
      var_dump($ra->getName());
    }
  }
}

echo "\n*** methods\n";

foreach ([
  new ReflectionMethod('SplObserver', 'update'),
  new ReflectionMethod('c', 'bar'),
  new ReflectionMethod('c', 'pbar'),
  new ReflectionMethod($closure, '__invoke'),
] as $idx => $rm) {
  foreach ($rm->getParameters() as $idx2 => $rp) {
    echo "** Method $idx - parameter $idx2\n";
    var_dump($rp->hasType());
    $ra = $rp->getType();
    if ($ra) {
      var_dump($ra->allowsNull());
      var_dump($ra->isBuiltin());
      var_dump($ra->getName());
    }
  }
}

echo "\n*** return types\n";

foreach ([
  new ReflectionMethod('SplObserver', 'update'),
  new ReflectionFunction('bar'),
  new ReflectionMethod('c', 'bar'),
  new ReflectionMethod('c', 'factory'),
  new ReflectionMethod('c', 'pfactory'),
  new ReflectionFunction($closure),
  new ReflectionMethod($closure, '__invoke'),
] as $idx => $rf) {
  echo "** Function/method return type $idx\n";
  var_dump($rf->hasReturnType());
  $ra = $rf->getReturnType();
  if ($ra) {
    var_dump($ra->allowsNull());
    var_dump($ra->isBuiltin());
    var_dump($ra->getName());
  }
}

echo "\n*** property types\n";

class PropTypeTest {
    public int $int;
    public string $string;
    public array $arr;
    public stdClass $std;
    public OtherThing $other;
    public $mixed;
}

$reflector = new ReflectionClass(PropTypeTest::class);

foreach ($reflector->getProperties() as $name => $property) {
    if ($property->hasType()) {
        printf("public %s $%s;\n",
            $property->getType()->getName(), $property->getName());
    } else printf("public $%s;\n", $property->getName());
}

echo "*** resolved property types\n";
$obj = new PropTypeTest;
$obj->std = new stdClass;
$r = (new ReflectionProperty($obj, 'std'))->getType();
var_dump($r->getName());
?>
--EXPECT--
*** functions
** Function 0 - Parameter 0
bool(true)
bool(false)
bool(false)
string(8) "stdClass"
** Function 0 - Parameter 1
bool(true)
bool(false)
bool(true)
string(5) "array"
** Function 0 - Parameter 2
bool(true)
bool(false)
bool(true)
string(8) "callable"
** Function 0 - Parameter 3
bool(true)
bool(false)
bool(true)
string(6) "string"
** Function 0 - Parameter 4
bool(true)
bool(false)
bool(true)
string(4) "bool"
** Function 0 - Parameter 5
bool(true)
bool(false)
bool(true)
string(3) "int"
** Function 0 - Parameter 6
bool(true)
bool(false)
bool(true)
string(5) "float"
** Function 0 - Parameter 7
bool(true)
bool(false)
bool(false)
string(11) "NotExisting"
** Function 0 - Parameter 8
bool(true)
bool(true)
bool(false)
string(8) "stdClass"
** Function 0 - Parameter 9
bool(false)
** Function 1 - Parameter 0
bool(true)
bool(false)
bool(false)
string(4) "Test"

*** methods
** Method 0 - parameter 0
bool(true)
bool(false)
bool(false)
string(10) "SplSubject"
** Method 1 - parameter 0
bool(true)
bool(false)
bool(false)
string(4) "self"
** Method 2 - parameter 0
bool(true)
bool(false)
bool(false)
string(6) "parent"
** Method 3 - parameter 0
bool(true)
bool(false)
bool(false)
string(4) "Test"

*** return types
** Function/method return type 0
bool(false)
** Function/method return type 1
bool(true)
bool(false)
bool(false)
string(8) "stdClass"
** Function/method return type 2
bool(true)
bool(false)
bool(true)
string(3) "int"
** Function/method return type 3
bool(true)
bool(false)
bool(false)
string(4) "self"
** Function/method return type 4
bool(true)
bool(false)
bool(false)
string(6) "parent"
** Function/method return type 5
bool(true)
bool(false)
bool(false)
string(4) "Test"
** Function/method return type 6
bool(true)
bool(false)
bool(false)
string(4) "Test"

*** property types
public int $int;
public string $string;
public array $arr;
public stdClass $std;
public OtherThing $other;
public $mixed;
*** resolved property types
string(8) "stdClass"
