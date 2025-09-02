--TEST--
Indirect method call with chaining
--FILE--
<?php

class foo {
    public $x = 'testing';

    public function bar() {
        return "foo";
    }
    public function baz() {
        return new self;
    }
    static function xyz() {
    }
}

var_dump((new foo())->bar());               // string(3) "foo"
var_dump((new foo())->baz()->x);            // string(7) "testing"
var_dump((new foo())->baz()->baz()->bar()); // string(3) "foo"
var_dump((new foo())->xyz());               // NULL
(new foo())->www();

?>
--EXPECTF--
string(3) "foo"
string(7) "testing"
string(3) "foo"
NULL

Fatal error: Uncaught Error: Call to undefined method foo::www() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
