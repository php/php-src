--TEST--
Lazy objects: Foreach initializes object
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a;
    public int $b {
        get { return $this->b; }
        set(int $value) { $this->b = $value; }
    }
    public int $c {
        get { return $this->a + 2; }
    }
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
        $this->b = 2;
        $this->d = 4;
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

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Ghost (init exception):\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    throw new \Exception();
});

try {
    var_dump(json_encode($obj));
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

print "# Proxy (init exception):\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    throw new \Exception();
});

try {
    var_dump(json_encode($obj));
} catch (\Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECT--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
string(1) "c"
int(3)
string(1) "d"
int(4)
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
string(1) "b"
int(2)
string(1) "c"
int(3)
# Ghost (init exception):
Exception: 
# Proxy (init exception):
Exception:
