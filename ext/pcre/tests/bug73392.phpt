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

try {
    preg_replace_callback_array(
        array(
            "/a/" => 'b',
            "/b/" => function () { return "c"; },
            "/c/" => new Rep,
            "reporting" => array("Foo", "rep"),
            "a1" => array("Foo", "rep"),
        ),
        'a'
    );
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
preg_replace_callback_array(): Regular expression delimiter cannot be alphanumeric or a backslash
