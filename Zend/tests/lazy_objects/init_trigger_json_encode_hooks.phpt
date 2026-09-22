--TEST--
Lazy objects: json_encode initializes object
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

var_dump(json_encode($obj));

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(json_encode($obj));

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
string(25) "{"a":1,"b":2,"c":3,"d":4}"
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(25) "{"a":1,"b":2,"c":3,"d":4}"
# Ghost (init exception):
Exception: 
# Proxy (init exception):
Exception:
