--TEST--
Constructor promotion without body (without any args)
--FILE--
<?php

class Point {
    public function __construct();
}

?>
--EXPECTF--
Fatal error: Method Point::__construct() must contain body in %s on line %d
