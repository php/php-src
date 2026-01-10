--TEST--
Lazy Objects: GH-15999 002: Assigned value is changed during lazy object initialization
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
    global $p;
    $p = null;
});

$p = new stdClass;
var_dump($o->s = $p);
var_dump($o->s);

print "# Proxy:\n";

$r = new ReflectionClass(C::class);

$o = $r->newLazyProxy(function ($obj) {
    global $p;
    $p = null;
    return new C();
});

$p = new stdClass;
var_dump($o->s = $p);
var_dump($o->s);

?>
==DONE==
--EXPECTF--
# Ghost:
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
# Proxy:
string(13) "C::__destruct"
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
==DONE==
string(13) "C::__destruct"
