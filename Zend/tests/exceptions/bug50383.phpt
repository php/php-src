--TEST--
Bug #50383 (Exceptions thrown in __call / __callStatic do not include file and line in trace)
--FILE--
<?php

class myClass {
    public static function __callStatic($method, $args) {
        throw new Exception("Missing static method '$method'\n");
    }
    public function __call($method, $args) {
        throw new Exception("Missing method '$method'\n");
    }
}

function thrower() {
    myClass::ThrowException();
}
function thrower2() {
    $x = new myClass;
    $x->foo();
}

try {
    thrower();
} catch(Exception $e) {
    print $e->getMessage();
    print_r($e->getTrace());
}

try {
    thrower2();
} catch (Exception $e) {
    print $e->getMessage();
    print_r($e->getTrace());
}

?>
--EXPECTF--
Missing static method 'ThrowException'
Array
(
    [0] => Array
        (
            [file] => %s
            [line] => 13
            [function] => __callStatic
            [class] => myClass
            [type] => ::
            [args] => Array
                (
                    [0] => ThrowException
                    [1] => Array
                        (
                        )

                )

        )

    [1] => Array
        (
            [file] => %s
            [line] => 21
            [function] => thrower
            [args] => Array
                (
                )

        )

)
Missing method 'foo'
Array
(
    [0] => Array
        (
            [file] => %s
            [line] => 17
            [function] => __call
            [class] => myClass
            [type] => ->
            [args] => Array
                (
                    [0] => foo
                    [1] => Array
                        (
                        )

                )

        )

    [1] => Array
        (
            [file] => %s
            [line] => 28
            [function] => thrower2
            [args] => Array
                (
                )

        )

)
