--TEST--
Object serialization with references
--FILE--
<?php
function gen() {
    $s = new stdClass;
    $r = new stdClass;
    $r->a = [$s];
    $r->b = $r->a;
    return $r;
}
var_dump(serialize(gen()));
?>
--EXPECTF--
string(78) "O:8:"stdClass":2:{s:1:"a";a:1:{i:0;O:8:"stdClass":0:{}}s:1:"b";a:1:{i:0;r:3;}}"


