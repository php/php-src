--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

// Ensuring that inconsistent constructor use results in an error to avoid
// problems creeping in.

trait TNew {
    public function __construct() {
        echo "TNew executed\n";
    }
}

class ReportCollision {
    use TNew;

	public function ReportCollision() {
	    echo "ReportCollision executed\n";
	}
}


echo "ReportCollision: ";
$o = new ReportCollision;
--EXPECTF--
Fatal error: ReportCollision has colliding constructor definitions coming from traits in %s on line %d
