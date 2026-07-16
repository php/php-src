--TEST--
A parameter cannot shadow the extension receiver variable
--FILE--
<?php
class C {}

extension C $c {
    public function m(int $c): int { return $c; }
}
?>
--EXPECTF--
Fatal error: Cannot redeclare extension receiver $c as a parameter in %s on line %d