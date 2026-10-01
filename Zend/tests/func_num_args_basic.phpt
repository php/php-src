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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

test3(1,2);

call_user_func("test1");
try {
    call_user_func("test3", 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
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
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
int(0)
int(1)
ArgumentCountError: Too few arguments to function test2(), 0 passed in %s on line %d and exactly 1 expected
int(2)
int(0)
ArgumentCountError: Too few arguments to function test3(), 1 passed in %s on line %d and exactly 2 expected
int(2)
int(1)
Error: func_num_args() must be called from a function context
Done
