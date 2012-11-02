--TEST--
Instance invoke 
--FILE--
<?php

class Foo
{
    public $var = "foo";
    public function dummy() {
        var_dump(__METHOD__);
    }
    public function __invoke() {
        return $this;
    }
    public function __get($name) {
        return $name;
    }
}

(new Foo())()->dummy();
var_dump((new Foo())->var);
var_dump((new Foo())->noexists);

--EXPECT--
string(10) "Foo::dummy"
string(3) "foo"
string(8) "noexists"
