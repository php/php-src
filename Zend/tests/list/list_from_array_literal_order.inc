<?php

class D {
    public function __construct(public string $n) { echo "construct $this->n\n"; }
    public function __destruct() { echo "destruct $this->n\n"; $GLOBALS['dead'] = true; }
    public function __toString(): string { return $this->n; }
}
function v(string $n) { echo "value $n\n"; return $n; }
function k(string $n) { echo "key $n\n"; return 0; }

echo "-- values before targets\n";
$x = [];
[$x[k('a')], $x[k('b')]] = [v('1'), v('2')];

echo "-- extra values\n";
[$a] = [new D('a'), new D('x'), new D('y')];
echo "assigned\n";
unset($a);

echo "-- values nobody takes live until the assignments are done\n";
function untaken_variable() {
    $x = new D('old x');
    $y = new D('y');
    [, $x, $y] = [$x, 1, 2];
    echo "assigned\n";
}
untaken_variable();

echo "-- a value the target does not keep lives until the assignments are done\n";
final class Drop implements ArrayAccess {
    public function offsetExists($o): bool { return true; }
    public function offsetGet($o): mixed { return null; }
    public function offsetSet($o, $v): void { echo "set $o\n"; }
    public function offsetUnset($o): void {}
}
$drop = new Drop;
[$drop['k'], $drop['l']] = [new D('k'), new D('l')];
echo "assigned\n";
$b = new D('old b');
[$a, $a, $b] = [new D('first'), new D('second'), 3];
echo "assigned\n";
unset($a);

echo "-- the same with a destructor that throws\n";
final class Boom { public function __destruct() { throw new Exception('boom'); } }
$b = 0;
try {
    [$a, $a, $b] = [new Boom, new stdClass, 42];
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($b);
unset($a);

echo "-- two targets that are references to each other\n";
$a = null; $b = null; $c = 0;
$a = &$b;
try {
    [$a, $b, $c] = [new Boom, new stdClass, 42];
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($c);
unset($a, $b);

echo "-- a variable that is a reference to a typed property\n";
final class S { public string $s = ''; }
$obj = new S;
$r = &$obj->s;
[$r, $drop['y']] = [new D('coerced'), 1];
echo "assigned\n";
unset($r, $obj);

echo "-- pending values when a target expression throws\n";
function boom() { throw new Exception('boom'); }
try {
    [$arr[boom()], $b, $c] = [new D('v1'), new D('v2'), new D('v3')];
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

echo "-- a variable is copied before any assignment\n";
$o = new D('o');
[$o, $p] = [new D('new o'), $o];
echo "assigned\n";
var_dump($o->n, $p->n);
unset($o, $p);

echo "-- exception in a later value\n";
try {
    [$a, $b, $c] = [new D('first'), new D('second'), boom()];
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

echo "-- exception in an assignment\n";
class T { public int $i = 1; }
$t = new T;
try {
    [$t->i, $b] = ['not an int', new D('after the throw')];
} catch (TypeError $e) {
    echo get_class($e), "\n";
}
var_dump($t->i);

echo "-- a destructor that throws during the release after the assignments\n";
final class BoomNamed extends D {
    public function __destruct() { echo "destruct $this->n throws\n"; throw new Exception("boom $this->n"); }
}
try {
    [$drop['a'], $drop['b']] = [new BoomNamed('a'), new D('b')];
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

echo "done\n";
