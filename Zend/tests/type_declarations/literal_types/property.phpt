--TEST--
Literal types: typed properties with literal types
--FILE--
<?php
class C {
    public 1|2|3 $p = 1;
    public 'on'|'off' $state = 'off';
}

$c = new C();
var_dump($c->p, $c->state);

$c->p = 3;
$c->state = 'on';
var_dump($c->p, $c->state);

$c->p = "2";
var_dump($c->p);

try {
    $c->p = 9;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $c->state = 'maybe';
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(1)
string(3) "off"
int(3)
string(2) "on"
int(2)
Cannot assign int to property C::$p of type 1|2|3
Cannot assign string to property C::$state of type 'on'|'off'
