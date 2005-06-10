--TEST--
Bug #30162 (Catching exception in constructor couses lose of $this)
--FILE--
<?php
class FIIFO {

	public function __construct() {
		$this->x = "x";
		throw new Exception;
	}

}

class hariCow extends FIIFO {

	public function __construct() {
		try {
			parent::__construct();
		} catch(Exception $e) {
		}
		$this->y = "y";
		try {
			$this->z = new FIIFO;
		} catch(Exception $e) {
		}
	}
	
	public function __toString() {
		return "Rusticus in asino sedet.";
	}

}

try {
	$db = new FIIFO();
} catch(Exception $e) {
}
var_dump($db);

$db = new hariCow;

var_dump($db);
?>
--EXPECTF--
Notice: Undefined variable: db in %sbug30162.php on line 35
NULL
object(hariCow)#1 (2) {
  ["x"]=>
  string(1) "x"
  ["y"]=>
  string(1) "y"
}
