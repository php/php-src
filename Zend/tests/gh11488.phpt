--TEST--
GH-11488: "Optional parameter before required" warning for union nullable type
--FILE--
<?php
class C {
    public function test(
        string|null $a = null,
        $b,
    ) {}
}
?>
--EXPECTF--
Deprecated: Optional parameter $a declared before required parameter $b is implicitly treated as a required parameter in %s on line %d
