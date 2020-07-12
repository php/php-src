--TEST--
func_get_arg() tests
--FILE--
<?php

function test1() {
    try {
        var_dump(func_get_arg(-10));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    try {
        var_dump(func_get_arg(0));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(func_get_arg(1));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

function test2($a) {
    try {
        var_dump(func_get_arg(0));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(func_get_arg(1));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

function test3($a, $b) {
    try {
        var_dump(func_get_arg(0));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(func_get_arg(1));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(func_get_arg(2));
    } catch (\Error $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

test1();
test1(10);
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
        try {
            var_dump(func_get_arg(0));
        } catch (\Error $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
        try {
            var_dump(func_get_arg(1));
        } catch (\Error $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
    }
}

test::test1(1);
try {
    var_dump(func_get_arg(1));
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
func_get_arg(): Argument #1 ($position) must be greater than or equal to 0
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
func_get_arg(): Argument #1 ($position) must be greater than or equal to 0
int(10)
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
int(1)
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
Exception: Too few arguments to function test2(), 0 passed in %s002.php on line %d and exactly 1 expected
int(1)
int(2)
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
func_get_arg(): Argument #1 ($position) must be greater than or equal to 0
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
Exception: Too few arguments to function test3(), 1 passed in %s on line %d and exactly 2 expected
int(1)
int(2)
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
int(1)
func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
func_get_arg() cannot be called from the global scope
Done
