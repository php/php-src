--TEST--
Bug #60139 (Anonymous functions create cycles not detected by the GC)
--INI--
zend.enable_gc=1
--FILE--
<?php
class Foo {
    public $x;

    public function __construct() {
        $this->x = function() {};
    }
}

class Bar {
    public $x;

    public function __construct() {
        $self = $this;
        $this->x = function() use ($self) {};
    }
}

gc_collect_cycles();
new Foo;
var_dump(gc_collect_cycles());
new Bar;
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(2)
int(2)
