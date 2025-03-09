--TEST--
Private properties are included in equality checks
--FILE--
<?php

data class Length {
    public function __construct(private float $length) {}
}

$l1 = new Length(1.0);
$l2 = new Length(1.0);
$l3 = new Length(2.0);
var_dump($l1 === $l2);
var_dump($l1 !== $l3);
?>
--EXPECT--
bool(true)
bool(true)
