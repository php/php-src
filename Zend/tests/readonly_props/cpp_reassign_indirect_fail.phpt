--TEST--
Promoted readonly property reassignment in constructor - indirect reassignment not allowed
--FILE--
<?php

// Reassignment is NOT allowed in methods called by the constructor
class CalledMethod {
    public function __construct(
        public readonly string $prop = 'default',
    ) {
        try {
            $this->initProp();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }

    private function initProp(): void {
        $this->prop = 'from method';
    }
}

$cm = new CalledMethod();
var_dump($cm->prop);

// Reassignment is NOT allowed in closures called by the constructor
class ClosureInConstructor {
    public function __construct(
        public readonly string $prop = 'default',
    ) {
        $fn = function() {
            $this->prop = 'from closure';
        };
        try {
            $fn();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$cc = new ClosureInConstructor();
var_dump($cc->prop);

?>
--EXPECT--
Cannot modify readonly property CalledMethod::$prop
string(7) "default"
Cannot modify readonly property ClosureInConstructor::$prop
string(7) "default"
