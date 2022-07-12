--TEST--
Bug #73392 (A use-after-free in zend allocator management)
--FILE--
<?php
class Rep {
    public function __invoke() {
        return "d";
    }
}
class Foo {
    public static function rep($rep) {
        return "ok";
    }
}
function b() {
    return "b";
}
var_dump(preg_replace_callback_array(
    array(
        "/a/" => 'b',	"/b/" => function () { return "c"; }, "/c/" => new Rep, "reporting" => array("Foo", "rep"),  "a1" => array("Foo", "rep"),
    ), 'a'));
?>
--EXPECTF--
Warning: preg_replace_callback_array(): Delimiter must not be alphanumeric, backslash, or NUL in %sbug73392.php on line %d
NULL
