--TEST--
Testing __call and __callstatic
--FILE--
<?php

class a {
    public function __call($a, $b) {
        print "__call: ". $a ."\n";
    }
    static public function __callStatic($a, $b) {
        print "__callstatic: ". $a ."\n";
    }
    public function baz() {
        self::Bar();
    }
}


$a = new a;

$b = 'Test';
$a::$b();
$a->$b();

$a->baz();

a::Foo();

?>
--EXPECT--
__callstatic: Test
__call: Test
__call: Bar
__callstatic: Foo
