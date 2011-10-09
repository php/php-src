--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

// Ensuring that inconsistent constructor use results in an error to avoid
// problems creeping in.

trait TLegacy {
    public function ReportCollision() {
        echo "TLegacy executed\n";
    }
}

class ReportCollision {
    use TLegacy;
	
	public function __construct() {
	    echo "ReportCollision executed\n";
	}
}


echo "ReportCollision: ";
$o = new ReportCollision;


--EXPECTF--
Fatal error: ReportCollision has colliding constructor definitions coming from traits in %s on line %d