--TEST--
Property capture allowed with parent constructor
--FILE--
<?php

class Base {
    public $baseProperty = 1;
    public function __construct() {
        echo 'This will not be called';
    }
}

$captured = 2;
$anon = new class use($captured) extends Base {};

var_dump($anon->baseProperty);
var_dump($anon->captured);

?>
--EXPECT--
int(1)
int(2)
