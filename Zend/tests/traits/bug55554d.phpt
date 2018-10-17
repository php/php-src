--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

// Test mixed constructors from different traits, we are more strict about
// these cases, since that can lead to un-expected behavior.
// It is not consistent with the normal constructor handling, but
// here we have a chance to be more strict for the new traits.

trait TNew {
    public function __construct() {
        echo "TNew executed\n";
    }
}

trait TLegacy {
    public function ReportCollision() {
        echo "ReportCollision executed\n";
    }
}

class ReportCollision {
    use TNew, TLegacy;
}

$o = new ReportCollision;
--EXPECTF--
Fatal error: ReportCollision has colliding constructor definitions coming from traits in %s on line %d
