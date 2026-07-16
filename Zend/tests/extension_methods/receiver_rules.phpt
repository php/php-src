--TEST--
$this is banned in extension methods
--FILE--
<?php
class C { public int $v = 7; }

extension C $c {
    public function m(): int { return $this->v; }
}
?>
--EXPECTF--
Fatal error: Cannot use $this in an extension method (use the declared receiver $c) in %s on line %d