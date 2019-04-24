--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

// Ensuring that the collision still occurs as expected.

trait TC1 {
    public function ReportCollision() {
        echo "TC1 executed\n";
    }
}

trait TC2 {
    public function ReportCollision() {
        echo "TC1 executed\n";
    }
}

class ReportCollision {
	use TC1, TC2;
}


echo "ReportCollision: ";
$o = new ReportCollision;
--EXPECTF--
Fatal error: Trait method TC2::ReportCollision has not been applied as ReportCollision::ReportCollision, because of collision with TC1::ReportCollision in %s on line %d
