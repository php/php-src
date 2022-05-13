--TEST--
Bug #30162 (Catching exception in constructor couses lose of $this)
--FILE--
<?php
#[AllowDynamicProperties]
class FIIFO {

    public function __construct() {
        $this->x = "x";
        throw new Exception;
    }

}

#[AllowDynamicProperties]
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
Warning: Undefined variable $db in %s on line %d
NULL
object(hariCow)#%d (2) {
  ["x"]=>
  string(1) "x"
  ["y"]=>
  string(1) "y"
}
