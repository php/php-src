--TEST--
Testing method name case
--FILE--
<?php

class Foo {
    public function __call($a, $b) {
        print "nonstatic\n";
        var_dump($a);
    }
    static public function __callStatic($a, $b) {
        print "static\n";
        var_dump($a);
    }
    public function test() {
        $this->fOoBaR();
        self::foOBAr();
        $this::fOOBAr();
    }
}

$a = new Foo;
$a->test();
$a::bAr();
foo::BAZ();

?>
--EXPECTF--
nonstatic
string(%d) "fOoBaR"
nonstatic
string(%d) "foOBAr"
nonstatic
string(%d) "fOOBAr"
static
string(%d) "bAr"

Fatal error: Uncaught Error: Class "foo" not found in %s:%d
Stack trace:
#%d {main}
  thrown in %s on line %d
