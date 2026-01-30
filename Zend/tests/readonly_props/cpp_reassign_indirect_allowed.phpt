--TEST--
Promoted readonly property reassignment in constructor - indirect reassignment allowed
--FILE--
<?php

// Reassignment IS allowed in methods called by the constructor
class CalledMethod {
    public function __construct(
        public readonly string $prop = 'default',
    ) {
        $this->initProp();
    }

    private function initProp(): void {
        $this->prop = 'from method';
    }
}

$cm = new CalledMethod();
var_dump($cm->prop);

// Reassignment IS allowed in closures called by the constructor
class ClosureInConstructor {
    public function __construct(
        public readonly string $prop = 'default',
    ) {
        $fn = function() {
            $this->prop = 'from closure';
        };
        $fn();
    }
}

$cc = new ClosureInConstructor();
var_dump($cc->prop);

// But second reassignment still fails
class MultipleReassign {
    public function __construct(
        public readonly string $prop = 'default',
    ) {
        $this->initProp();
        try {
            $this->initProp();  // Second call - should fail
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }

    private function initProp(): void {
        $this->prop = 'from method';
    }
}

$mr = new MultipleReassign();
var_dump($mr->prop);

?>
--EXPECT--
string(11) "from method"
string(12) "from closure"
Cannot modify readonly property MultipleReassign::$prop
string(11) "from method"
