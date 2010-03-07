--TEST--
Bug #50383 (Exceptions thrown in __call / __callStatic do not include file and line in trace)
--FILE--
<?php

class myClass {
	public function __call($method, $args) {
		throw new Exception("Missing method '$method'\n");
	}
}

function thrower2() {
	$x = new myClass;
	$x->foo();
}

try {
	thrower2();
} catch (Exception $e) {
	print $e->getMessage();
	print_r($e->getTrace());
}

?>
--EXPECTF--
Missing method 'foo'
Array
(
    [0] => Array
        (
            [file] => %s
            [line] => 11
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
            [line] => 11
            [function] => foo
            [class] => myClass
            [type] => ->
            [args] => Array
                (
                )

        )

    [2] => Array
        (
            [file] => %s
            [line] => 15
            [function] => thrower2
            [args] => Array
                (
                )

        )

)
