--TEST--
Bug #37676 (Object warnings)
--FILE--
<?php
class FooArray implements ArrayAccess {
    private $s = [];
    function __construct(array $a) { $this->s = $a; }
    function offsetSet ($k, $v) { $this->s[$k] = $v; }
    function &offsetGet ($k) { return $this->s[$k]; }
    function offsetExists ($k) { return isset($this->s[$k]); }
    function offsetUnset ($k) { unset($this->s[$k]); }
}

$a = [1, 2, 3];
$fa = new FooArray($a);

$fa[0][1];
$fa[4][1];
?>
--EXPECTF--
Warning: Variable of type int does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d
