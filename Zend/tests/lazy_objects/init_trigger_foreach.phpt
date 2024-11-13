--TEST--
Lazy objects: Foreach initializes object
--FILE--
<?php

class C {
    public int $a;
    public int $b;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
        $this->b = 2;
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Ghost (by ref):\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

foreach ($obj as $prop => &$value) {
    var_dump($prop, $value);
}

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Proxy (by ref):\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

foreach ($obj as $prop => &$value) {
    var_dump($prop, $value);
}

print "# Ghost (init failure)\n";

$fail = true;
$obj = $reflector->newLazyGhost(function ($obj) use (&$fail) {
    if ($fail) {
        throw new Exception("initializer");
    } else {
        var_dump("initializer");
        $obj->__construct();
    }
});

try {
    foreach ($obj as $prop => $value) {
        var_dump($prop, $value);
    }
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$fail = false;
foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Ghost (init failure, by ref)\n";

$fail = true;
$obj = $reflector->newLazyGhost(function ($obj) use (&$fail) {
    if ($fail) {
        throw new Exception("initializer");
    } else {
        var_dump("initializer");
        $obj->__construct();
    }
});

try {
    foreach ($obj as $prop => &$value) {
        var_dump($prop, $value);
    }
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

$fail = false;
foreach ($obj as $prop => &$value) {
    var_dump($prop, $value);
}

?>
--EXPECT--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
# Ghost (by ref):
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
# Proxy (by ref):
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
# Ghost (init failure)
Exception: initializer
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
# Ghost (init failure, by ref)
Exception: initializer
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
