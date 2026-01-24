--TEST--
PFA RFC examples: "Scoping" section
--FILE--
<?php

require 'rfc_examples.inc';

function foo(int $i, int $j = 0): string {
    return "$i $j";
}

class Foo {
    static function bar(int $i, int $j): string {
        return "$i $j";
    }
}

class C {
    function f($a) {
        return self::class;
    }
}
class CSubClass extends C {
    function g($a) {
        return self::class;
    }

    function h($a): \Closure {
        return parent::f(?);
    }
}
class Unrelated {}

$tests = [
    'Static closure 1' => [
        foo(?, ?),
        static fn(int $i, int $j = 0): string => foo($i, $j),
    ],
    'Static closure 2' => [
        Foo::bar(?, ?),
        static fn(int $i, int $j): string => Foo::bar($i, $j),
    ],
    'Static closure 3' => [
        foo(?, ?)(1, ?),
        static fn(int $j = 0): string => foo(1, $j),
    ],
    'Static closure 4' => [
        foo(...)(?),
        static fn(int $i): string => foo($i, 0),
    ],
];

check_equivalence($tests);

$c = new C();
$f = $c->f(?);

echo "# Cannot unbind \$this:\n";
var_dump($f->bindTo(null, C::class)); // Warning: Cannot unbind $this of method, this will be an error in PHP 9 (returns null)

echo "# Cannot rebind scope:\n";
var_dump($f->bindTo($c, CSubClass::class)); // Warning: Cannot rebind scope of closure created from method, this will be an error in PHP 9 (returns null)

echo "# Can rebind \$this with subclass:\n";
var_dump($f->bindTo(new CSubClass, C::class)); // Allowed

echo "# Cannot rebind \$this with unrelated class:\n";
$f = $f->bindTo(new Unrelated, C::class); // Warning: Cannot bind method C::{closure:/path/to/test.php:11}() to object of class Unrelated, this will be an error in PHP 9 (returns null)

echo "# self resolution:\n";
$c = new CSubClass();
var_dump($c->f(?)(1)); // string(1) "C"
var_dump($c->g(?)(1)); // string(9) "CSubClass"
var_dump($c->h(1)(1)); // string(1) "C"

?>
--EXPECTF--
# Static closure 1: Ok
# Static closure 2: Ok
# Static closure 3: Ok
# Static closure 4: Ok
# Cannot unbind $this:

Warning: Cannot unbind $this of method, this will be an error in PHP 9 in %s on line %d
NULL
# Cannot rebind scope:

Warning: Cannot rebind scope of closure created from method, this will be an error in PHP 9 in %s on line %d
NULL
# Can rebind $this with subclass:
object(Closure)#%d (5) {
  ["name"]=>
  string(%d) "{closure:pfa:%s}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(53)
  ["this"]=>
  object(CSubClass)#%d (0) {
  }
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
}
# Cannot rebind $this with unrelated class:

Warning: Cannot bind method C::{closure:pfa:%s}() to object of class Unrelated, this will be an error in PHP 9 in %s on line %d
# self resolution:
string(1) "C"
string(9) "CSubClass"
string(1) "C"
