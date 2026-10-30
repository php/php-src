--TEST--
Assigning to a typed property must not use the object after coercion user code released it
--FILE--
<?php

/* Unlike GH-10169, the assignments below are primed first with a harmless
 * value, so that the releasing ones are executed through the cached fast path
 * and not through zend_std_write_property(). */

class Test {
    public string $s = "a";
    public int $i = 0;

    public function set($v) {
        $this->s = $v;
        return $this->s;
    }
}

class Releaser {
    public function __toString(): string {
        unset($GLOBALS['a'][0]);
        return "5";
    }
}

function attempt(Closure $fn) {
    try {
        $fn();
        echo "assigned\n";
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

function assign_tmp(array &$a, bool $release) { $a[0]->s = $release ? new Releaser : "x"; }
function assign_cv(array &$a, $v) { $a[0]->s = $v; }
function assign_int(array &$a, $v) { $a[0]->i = $v; }

$a = [new Test];
assign_tmp($a, false);
assign_cv($a, "x");
assign_int($a, 1);

echo "--- __toString()\n";
for ($k = 0; $k < 3; $k++) {
    $a = [new Test];
    attempt(function () use (&$a) { assign_tmp($a, true); });
    $a = [new Test];
    $v = new Releaser;
    attempt(function () use (&$a, $v) { assign_cv($a, $v); });
}

echo "--- error handler\n";
set_error_handler(function () {
    unset($GLOBALS['a'][0]);
    return true;
});
for ($k = 0; $k < 3; $k++) {
    $a = [new Test];
    attempt(function () use (&$a) { assign_int($a, "1.5"); });
}

echo "--- class type: the object is not coerced\n";
class Foo {}
class Holder {
    public Foo $foo;
    public string|Foo $u = "";
}
$a = [new Holder];
for ($k = 0; $k < 3; $k++) {
    $a[0]->foo = new Foo;
    var_dump($a[0]->foo instanceof Foo);
}

echo "--- union with string: the object is coerced\n";
function assign_union(array &$a, $v) { $a[0]->u = $v; }
$a = [new Holder];
assign_union($a, "x");
for ($k = 0; $k < 3; $k++) {
    $a = [new Holder];
    attempt(function () use (&$a) { assign_union($a, new Releaser); });
}

echo "--- \$this keeps the object alive\n";
for ($k = 0; $k < 3; $k++) {
    $a = [new Test];
    var_dump($a[0]->set(new Releaser));
}

echo "--- static property\n";
class Stringy {
    public function __toString(): string { return "str"; }
}
class Sta {
    public static string $s = "a";
    public static int $i = 0;
    public static ?Foo $foo = null;
}
for ($k = 0; $k < 3; $k++) {
    Sta::$s = 12;
    var_dump(Sta::$s);
    Sta::$s = new Stringy;
    var_dump(Sta::$s);
    Sta::$i = "5";
    var_dump(Sta::$i);
    Sta::$i = "1.5";
    var_dump(Sta::$i);
    Sta::$foo = new Foo;
    var_dump(Sta::$foo instanceof Foo);
    attempt(function () { Sta::$i = "x"; });
    attempt(function () { Sta::$foo = new stdClass; });
    var_dump(Sta::$i);
}

echo "--- object stays alive\n";
$a = [new Test];
$a[0]->s = 12;
var_dump($a[0]->s);

?>
--EXPECT--
--- __toString()
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
Object was released while assigning to property Test::$s
--- error handler
Object was released while assigning to property Test::$i
Object was released while assigning to property Test::$i
Object was released while assigning to property Test::$i
--- class type: the object is not coerced
bool(true)
bool(true)
bool(true)
--- union with string: the object is coerced
Object was released while assigning to property Holder::$u
Object was released while assigning to property Holder::$u
Object was released while assigning to property Holder::$u
--- $this keeps the object alive
string(1) "5"
string(1) "5"
string(1) "5"
--- static property
string(2) "12"
string(3) "str"
int(5)
int(1)
bool(true)
Cannot assign string to property Sta::$i of type int
Cannot assign stdClass to property Sta::$foo of type ?Foo
int(1)
string(2) "12"
string(3) "str"
int(5)
int(1)
bool(true)
Cannot assign string to property Sta::$i of type int
Cannot assign stdClass to property Sta::$foo of type ?Foo
int(1)
string(2) "12"
string(3) "str"
int(5)
int(1)
bool(true)
Cannot assign string to property Sta::$i of type int
Cannot assign stdClass to property Sta::$foo of type ?Foo
int(1)
--- object stays alive
string(2) "12"
