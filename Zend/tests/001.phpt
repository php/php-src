--TEST--
func_num_args() tests
--FILE--
<?php

function test1() {
    var_dump(func_num_args());
}

function test2($a) {
    var_dump(func_num_args());
}

function test3($a, $b) {
    var_dump(func_num_args());
}

test1();
test2(1);
try {
    test2();
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

test3(1,2);

call_user_func("test1");
try {
    call_user_func("test3", 1);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
call_user_func("test3", 1, 2);

class test {
    static function test1($a) {
        var_dump(func_num_args());
    }
}

test::test1(1);

try {
    func_num_args();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--
int(0)
int(1)
Exception: Too few arguments to function test2(), 0 passed in %s on line %d and exactly 1 expected
int(2)
int(0)
Exception: Too few arguments to function test3(), 1 passed in %s on line %d and exactly 2 expected
int(2)
int(1)
func_num_args() must be called from a function context
Done
