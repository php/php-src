--TEST--
First write to an uninitialized typed property: write access, readonly, hooks and lazy objects
--FILE--
<?php

function attempt(callable $f, ...$args) {
    try {
        $f(...$args);
        echo "ok\n";
    } catch (Throwable $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

echo "--- protected(set) / private(set) from a foreign scope after warm-up\n";
class Avis {
    public protected(set) int $p;
    public private(set) int $q;
    public function init() { $this->p = 1; $this->q = 1; }
}
function w_p(Avis $o) { $o->p = 5; }
function w_q(Avis $o) { $o->q = 5; }
for ($k = 0; $k < 3; $k++) {
    $o = new Avis; attempt('w_p', $o); var_dump(isset($o->p));
    $o = new Avis; attempt('w_q', $o); var_dump(isset($o->q));
    $o = new Avis; $o->init(); var_dump($o->p, $o->q);
}

echo "--- the same closure bound to different scopes\n";
class Both {
    public protected(set) int $x;
}
$write = function (Both $o) { $o->x = 1; };
for ($k = 0; $k < 3; $k++) {
    $o = new Both;
    attempt($write, $o); var_dump(isset($o->x));
    $o = new Both;
    attempt(Closure::bind($write, null, Both::class), $o); var_dump(isset($o->x));
    $o = new Both;
    attempt($write, $o); var_dump(isset($o->x));
}

echo "--- readonly\n";
class RO {
    public function __construct(public readonly int $x) {}
}
class RO2 {
    public readonly int $x;
    public function init(int $v) { $this->x = $v; }
}
for ($k = 0; $k < 3; $k++) {
    $o = new RO2; $o->init(1); attempt([$o, 'init'], 2); var_dump($o->x);
    $o = new RO2; attempt(function() use ($o) { $o->x = 3; }); var_dump(isset($o->x));
    $o = new RO(7); var_dump($o->x);
}

echo "--- readonly: write into an uninitialized property in __clone()\n";
class Cl {
    public readonly int $a;
    public readonly int $b;
    public function __construct() { $this->a = 1; }
    public function __clone() { $this->b = 2; attempt([$this, 'again']); }
    public function again() { $this->b = 3; }
}
function foreign(Cl $c) { $c->b = 4; }
for ($k = 0; $k < 3; $k++) {
    $c = new Cl;
    $d = clone $c;
    var_dump($d->a, $d->b);
    attempt('foreign', $c);
    attempt('foreign', $d);
}

echo "--- hooks: write to the backing store from a hook\n";
class Hooked {
    public int $v {
        set { echo "set hook\n"; $this->v = $value * 2; }
    }
    public int $w {
        get => $this->w;
        set { $this->w = $value + 1; }
    }
}
for ($k = 0; $k < 3; $k++) {
    $o = new Hooked; $o->v = 5; var_dump($o->v);
    $o = new Hooked; $o->w = 5; var_dump($o->w);
    $o = new Hooked; attempt(function() use ($o) { $o->w = "x"; });
}

echo "--- lazy objects\n";
class Lazy {
    public int $a;
    public string $b;
    public function __construct() { echo "init\n"; $this->a = 1; }
}
function set_a(Lazy $o, $v) { $o->a = $v; }
function set_b(Lazy $o, $v) { $o->b = $v; }
$rc = new ReflectionClass(Lazy::class);
for ($k = 0; $k < 3; $k++) {
    $ghost = $rc->newLazyGhost(function (Lazy $o) { echo "ghost init\n"; $o->__construct(); });
    attempt('set_b', $ghost, "x"); var_dump($rc->isUninitializedLazyObject($ghost));
    $proxy = $rc->newLazyProxy(fn() => new Lazy);
    attempt('set_b', $proxy, "y"); var_dump($rc->isUninitializedLazyObject($proxy));
    $plain = new Lazy;
    attempt('set_b', $plain, "z"); var_dump($plain->b);
}

?>
--EXPECTF--
--- protected(set) / private(set) from a foreign scope after warm-up
Error: Cannot modify protected(set) property Avis::$p from global scope
bool(false)
Error: Cannot modify private(set) property Avis::$q from global scope
bool(false)
int(1)
int(1)
Error: Cannot modify protected(set) property Avis::$p from global scope
bool(false)
Error: Cannot modify private(set) property Avis::$q from global scope
bool(false)
int(1)
int(1)
Error: Cannot modify protected(set) property Avis::$p from global scope
bool(false)
Error: Cannot modify private(set) property Avis::$q from global scope
bool(false)
int(1)
int(1)
--- the same closure bound to different scopes
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
ok
bool(true)
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
ok
bool(true)
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
ok
bool(true)
Error: Cannot modify protected(set) property Both::$x from global scope
bool(false)
--- readonly
Error: Cannot modify readonly property RO2::$x
int(1)
Error: Cannot modify protected(set) readonly property RO2::$x from global scope
bool(false)
int(7)
Error: Cannot modify readonly property RO2::$x
int(1)
Error: Cannot modify protected(set) readonly property RO2::$x from global scope
bool(false)
int(7)
Error: Cannot modify readonly property RO2::$x
int(1)
Error: Cannot modify protected(set) readonly property RO2::$x from global scope
bool(false)
int(7)
--- readonly: write into an uninitialized property in __clone()
Error: Cannot modify readonly property Cl::$b
int(1)
int(2)
Error: Cannot modify protected(set) readonly property Cl::$b from global scope
Error: Cannot modify readonly property Cl::$b
Error: Cannot modify readonly property Cl::$b
int(1)
int(2)
Error: Cannot modify protected(set) readonly property Cl::$b from global scope
Error: Cannot modify readonly property Cl::$b
Error: Cannot modify readonly property Cl::$b
int(1)
int(2)
Error: Cannot modify protected(set) readonly property Cl::$b from global scope
Error: Cannot modify readonly property Cl::$b
--- hooks: write to the backing store from a hook
set hook
int(10)
int(6)
TypeError: Hooked::$w::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
set hook
int(10)
int(6)
TypeError: Hooked::$w::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
set hook
int(10)
int(6)
TypeError: Hooked::$w::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
--- lazy objects
ghost init
init
ok
bool(false)
init
ok
bool(false)
init
ok
string(1) "z"
ghost init
init
ok
bool(false)
init
ok
bool(false)
init
ok
string(1) "z"
ghost init
init
ok
bool(false)
init
ok
bool(false)
init
ok
string(1) "z"
