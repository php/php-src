--TEST--
Constructor promotion without body (with non-promoted arg)
--FILE--
<?php

class Point {
    public function __construct(public $x, public $y, $z);
}

?>
--EXPECTF--
Fatal error: Method Point::__construct() must contain body in %s on line %d
