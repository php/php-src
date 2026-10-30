--TEST--
Lazy objects: initializer must return the right type
--FILE--
<?php

class B {
    public int $b;
    public function __construct() {
        $this->b = 1;
    }
    public function __destruct() {
    }
}

class C extends B {
}

class D extends C {
    public int $b; // override
}

class E extends B {
    public function __destruct() { // override
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost initializer must return NULL or no value:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
    return new stdClass;
});

var_dump($obj);
try {
    var_dump($obj->a);
} catch (\Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
var_dump($obj);

print "# Proxy initializer must return an instance of a compatible class:\n";
print "## Valid cases:\n";

$tests = [
    [C::class, new C()],
    [C::class, new B()],
    [D::class, new B()],
];

foreach ($tests as [$class, $instance]) {
    $obj = (new ReflectionClass($class))->newLazyProxy(function ($obj) use ($instance) {
        var_dump("initializer");
        $instance->b = 1;
        return $instance;
    });

    printf("## %s vs %s\n", get_class($obj), is_object($instance) ? get_class($instance) : gettype($instance));
    var_dump($obj->b);
    var_dump($obj);
}

print "## Invalid cases:\n";

$tests = [
    [C::class, new stdClass],
    [C::class, new DateTime()],
    [C::class, null],
    [C::class, new D()],
    [E::class, new B()],
];

foreach ($tests as [$class, $instance]) {
    $obj = (new ReflectionClass($class))->newLazyProxy(function ($obj) use ($instance) {
        var_dump("initializer");
        return $instance;
    });

    try {
        printf("## %s vs %s\n", get_class($obj), is_object($instance) ? get_class($instance) : gettype($instance));
        var_dump($obj->a);
    } catch (\Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return $obj;
});

try {
    printf("## %s vs itself\n", get_class($obj));
    var_dump($obj->a);
} catch (\Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECTF--
# Ghost initializer must return NULL or no value:
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
string(11) "initializer"
TypeError: Lazy object initializer must return NULL or no value
lazy ghost object(C)#%d (0) {
  ["b"]=>
  uninitialized(int)
}
# Proxy initializer must return an instance of a compatible class:
## Valid cases:
## C vs C
string(11) "initializer"
int(1)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["b"]=>
    int(1)
  }
}
## C vs B
string(11) "initializer"
int(1)
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(B)#%d (1) {
    ["b"]=>
    int(1)
  }
}
## D vs B
string(11) "initializer"
int(1)
lazy proxy object(D)#%d (1) {
  ["instance"]=>
  object(B)#%d (1) {
    ["b"]=>
    int(1)
  }
}
## Invalid cases:
## C vs stdClass
string(11) "initializer"
TypeError: The real instance class stdClass is not compatible with the proxy class C. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
## C vs DateTime
string(11) "initializer"
TypeError: The real instance class DateTime is not compatible with the proxy class C. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
## C vs NULL
string(11) "initializer"
TypeError: Lazy proxy factory must return an instance of a class compatible with C, null returned
## C vs D
string(11) "initializer"
TypeError: The real instance class D is not compatible with the proxy class C. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
## E vs B
string(11) "initializer"
TypeError: The real instance class B is not compatible with the proxy class E. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
## C vs itself
string(11) "initializer"
Error: Lazy proxy factory must return a non-lazy object
