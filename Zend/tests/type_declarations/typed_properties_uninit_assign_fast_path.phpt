--TEST--
First write to an uninitialized typed property (ZEND_ASSIGN_OBJ fast path)
--FILE--
<?php

class Foo {}
class Bar extends Foo {}

class Plain {
    public int $i;
    public string $s;
    public ?Foo $foo;
    public int|string $u;
    public array $a;
    public float $f;
    public mixed $m;
    public ?int $n;
}

/* Each helper is called repeatedly so that the cache slot is primed and the
 * fast path is taken from the second call on. */
function set_i(Plain $o, $v) { $o->i = $v; }
function set_s(Plain $o, $v) { $o->s = $v; }
function set_foo(Plain $o, $v) { $o->foo = $v; }
function set_u(Plain $o, $v) { $o->u = $v; }
function set_a(Plain $o, $v) { $o->a = $v; }
function set_f(Plain $o, $v) { $o->f = $v; }
function ret(Plain $o, $v) { return $o->i = $v; }
function chain(Plain $o, $v) { return $o->s = $o->i = $v; }

function attempt(callable $f, ...$args) {
    try {
        $r = $f(...$args);
        var_dump($r);
    } catch (Throwable $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

echo "--- coercion, weak mode\n";
for ($k = 0; $k < 3; $k++) {
    $o = new Plain; attempt('set_i', $o, "2"); var_dump($o->i);
    $o = new Plain; attempt('set_i', $o, 2.0); var_dump($o->i);
    $o = new Plain; attempt('set_i', $o, true); var_dump($o->i);
    $o = new Plain; attempt('set_i', $o, "abc"); var_dump(isset($o->i));
    $o = new Plain; attempt('set_i', $o, null); var_dump(isset($o->i));
    $o = new Plain; attempt('set_s', $o, 12); var_dump($o->s);
}

echo "--- union, class and nullable types\n";
for ($k = 0; $k < 3; $k++) {
    $o = new Plain; attempt('set_u', $o, true); var_dump($o->u);
    $o = new Plain; attempt('set_u', $o, []); var_dump(isset($o->u));
    $o = new Plain; attempt('set_foo', $o, new Bar); var_dump($o->foo instanceof Bar);
    $o = new Plain; attempt('set_foo', $o, null); var_dump($o->foo);
    $o = new Plain; attempt('set_foo', $o, new stdClass); var_dump(isset($o->foo));
    $o = new Plain; attempt('set_f', $o, 3); var_dump($o->f);
}

echo "--- result value\n";
for ($k = 0; $k < 3; $k++) {
    $o = new Plain; attempt('ret', $o, "5");
    $o = new Plain; attempt('ret', $o, "x");
    $o = new Plain; attempt('chain', $o, "7"); var_dump($o->s, $o->i);
}

echo "--- reference and refcounted values\n";
for ($k = 0; $k < 3; $k++) {
    $o = new Plain;
    $str = str_repeat("a", 3) . $k;
    $arr = [$k, [$k]];
    $ref = &$arr;
    set_s($o, $str);
    set_a($o, $ref);
    $arr[] = 1;
    var_dump($o->s === $str, count($o->a), count($arr));
    unset($ref, $arr, $str);
    var_dump($o->s, $o->a);
}

echo "--- strict_types is taken from the calling file\n";
eval('declare(strict_types=1); function set_i_strict(Plain $o, $v) { $o->i = $v; }');
for ($k = 0; $k < 3; $k++) {
    $o = new Plain; attempt('set_i_strict', $o, "2"); var_dump(isset($o->i));
    $o = new Plain; attempt('set_i_strict', $o, 2); var_dump($o->i);
}

echo "--- unset() property goes through __set()\n";
class Magic {
    public int $x;
    public function __set($n, $v) { echo "__set($n)\n"; }
    public function drop() { unset($this->x); }
}
function set_x(Magic $o, $v) { $o->x = $v; }
for ($k = 0; $k < 3; $k++) {
    $o = new Magic; set_x($o, 1); var_dump($o->x);
    $o = new Magic; $o->drop(); set_x($o, 2); var_dump(isset($o->x));
}

echo "--- object released while coercing the value\n";
class Releaser {
    public function __toString(): string { unset($GLOBALS['victim']); return "5"; }
}
class Victim { public string $s; public function set($v) { $this->s = $v; } }
for ($k = 0; $k < 3; $k++) {
    $victim = new Victim;
    try {
        $victim->s = new Releaser;
        echo "assigned\n";
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
for ($k = 0; $k < 3; $k++) {
    $victim = new Victim;
    try {
        $victim->set(new Releaser);
        echo "assigned\n";
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
--- coercion, weak mode
NULL
int(2)
NULL
int(2)
NULL
int(1)
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
TypeError: Cannot assign null to property Plain::$i of type int
bool(false)
NULL
string(2) "12"
NULL
int(2)
NULL
int(2)
NULL
int(1)
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
TypeError: Cannot assign null to property Plain::$i of type int
bool(false)
NULL
string(2) "12"
NULL
int(2)
NULL
int(2)
NULL
int(1)
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
TypeError: Cannot assign null to property Plain::$i of type int
bool(false)
NULL
string(2) "12"
--- union, class and nullable types
NULL
int(1)
TypeError: Cannot assign array to property Plain::$u of type string|int
bool(false)
NULL
bool(true)
NULL
NULL
TypeError: Cannot assign stdClass to property Plain::$foo of type ?Foo
bool(false)
NULL
float(3)
NULL
int(1)
TypeError: Cannot assign array to property Plain::$u of type string|int
bool(false)
NULL
bool(true)
NULL
NULL
TypeError: Cannot assign stdClass to property Plain::$foo of type ?Foo
bool(false)
NULL
float(3)
NULL
int(1)
TypeError: Cannot assign array to property Plain::$u of type string|int
bool(false)
NULL
bool(true)
NULL
NULL
TypeError: Cannot assign stdClass to property Plain::$foo of type ?Foo
bool(false)
NULL
float(3)
--- result value
int(5)
TypeError: Cannot assign string to property Plain::$i of type int
string(1) "7"
string(1) "7"
int(7)
int(5)
TypeError: Cannot assign string to property Plain::$i of type int
string(1) "7"
string(1) "7"
int(7)
int(5)
TypeError: Cannot assign string to property Plain::$i of type int
string(1) "7"
string(1) "7"
int(7)
--- reference and refcounted values
bool(true)
int(2)
int(3)
string(4) "aaa0"
array(2) {
  [0]=>
  int(0)
  [1]=>
  array(1) {
    [0]=>
    int(0)
  }
}
bool(true)
int(2)
int(3)
string(4) "aaa1"
array(2) {
  [0]=>
  int(1)
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
bool(true)
int(2)
int(3)
string(4) "aaa2"
array(2) {
  [0]=>
  int(2)
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
--- strict_types is taken from the calling file
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
NULL
int(2)
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
NULL
int(2)
TypeError: Cannot assign string to property Plain::$i of type int
bool(false)
NULL
int(2)
--- unset() property goes through __set()
int(1)
__set(x)
bool(false)
int(1)
__set(x)
bool(false)
int(1)
__set(x)
bool(false)
--- object released while coercing the value
Object was released while assigning to property Victim::$s
Object was released while assigning to property Victim::$s
Object was released while assigning to property Victim::$s
assigned
assigned
assigned
