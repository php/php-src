--TEST--
Promoted readonly property reassignment in constructor - child preempt then parent ctor throws
--FILE--
<?php

class ParentNoCPP {
    public readonly string $prop;

    public function __construct(
        string $prop = 'parent default',
    ) {
        try {
            $this->prop = 'parent set';
        } catch (Error) {
            // readonly property set by child class
        }
    }
}

class ChildNoCPP extends ParentNoCPP {
    public function __construct() {
        $this->prop = 'child set';
        parent::__construct();
    }
}

class ParentCPP {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        try {
            $this->prop = 'parent set';
        } catch (Error) {
            // readonly property set by child class
        }
    }
}

class ChildCPP extends ParentCPP {
    public function __construct() {
        $this->prop = 'child set';
        try {
            parent::__construct();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$c = new ChildNoCPP();
var_dump($c->prop);

$c = new ChildCPP();
var_dump($c->prop);

?>
--EXPECT--
string(9) "child set"
Cannot modify readonly property ParentCPP::$prop
string(9) "child set"
