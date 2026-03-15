--TEST--
Promoted readonly property reassignment in constructor - child preempt then parent ctor throws
--FILE--
<?php

class ParentCPP {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        $this->prop = 'parent set';
    }
}

class ChildCPP extends ParentCPP {
    public function __construct() {
        $this->prop = 'child set';
        try {
            parent::__construct();
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$c = new ChildCPP();
var_dump($c->prop);

?>
--EXPECT--
Error: Cannot modify readonly property ParentCPP::$prop
string(9) "child set"
