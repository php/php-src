--TEST--
Promoted readonly property reassignment in constructor - non-promoted properties unchanged
--FILE--
<?php

// Non-promoted readonly properties still cannot be reassigned
class NonPromoted {
    public readonly string $prop;

    public function __construct() {
        $this->prop = 'first';
        try {
            $this->prop = 'second';  // Should fail - not a promoted property
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$np = new NonPromoted();
var_dump($np->prop);

// Test mixed: promoted and non-promoted in same class
class MixedProps {
    public readonly string $nonPromoted;

    public function __construct(
        public readonly string $promoted = 'default',
    ) {
        $this->nonPromoted = 'first';
        $this->promoted = 'reassigned';  // Allowed (promoted, first reassignment)
        try {
            $this->nonPromoted = 'second';  // Should fail (non-promoted)
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$m = new MixedProps();
var_dump($m->promoted, $m->nonPromoted);

?>
--EXPECT--
Cannot modify readonly property NonPromoted::$prop
string(5) "first"
Cannot modify readonly property MixedProps::$nonPromoted
string(10) "reassigned"
string(5) "first"
