--TEST--
Lazy Objects: GH-15999 001: Object is released during initialization
--FILE--
<?php

class C {
    public $s;
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

print "# Ghost:\n";

$r = new ReflectionClass(C::class);

$o = $r->newLazyGhost(function ($obj) {
    global $o;
    $o = null;
});
$p = new stdClass;

try {
    $o->s = $p;
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

print "# Proxy:\n";

$o = $r->newLazyProxy(function ($obj) {
    global $o;
    $o = null;
    return new C();
});
$p = new stdClass;

try {
    $o->s = $p;
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

print "# GC cycle:\n";

$o = $r->newLazyGhost(function ($obj) {
    global $o;
    $o->s = $o;
    $o = null;
    gc_collect_cycles();
});
$p = new stdClass;

$o->s = $p;
gc_collect_cycles();

print "# Nested error (ghost):\n";

$r = new ReflectionClass(C::class);

$o = $r->newLazyGhost(function ($obj) {
    global $o;
    $o = null;
    return new stdClass;
});
$p = new stdClass;

try {
    $o->s = $p;
} catch (Error $e) {
    do {
        printf("%s: %s\n", $e::class, $e->getMessage());
    } while ($e = $e->getPrevious());
}

print "# Nested error (proxy):\n";

$r = new ReflectionClass(C::class);

$o = $r->newLazyProxy(function ($obj) {
    global $o;
    $o = null;
    return new stdClass;
});
$p = new stdClass;

try {
    $o->s = $p;
} catch (Error $e) {
    do {
        printf("%s: %s\n", $e::class, $e->getMessage());
    } while ($e = $e->getPrevious());
}

?>
==DONE==
--EXPECT--
# Ghost:
string(13) "C::__destruct"
Error: Lazy object was released during initialization
# Proxy:
string(13) "C::__destruct"
Error: Lazy object was released during initialization
# GC cycle:
string(13) "C::__destruct"
# Nested error (ghost):
Error: Lazy object was released during initialization
TypeError: Lazy object initializer must return NULL or no value
# Nested error (proxy):
Error: Lazy object was released during initialization
TypeError: The real instance class stdClass is not compatible with the proxy class C. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
==DONE==
