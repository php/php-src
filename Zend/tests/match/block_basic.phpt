--TEST--
Basic match blocks
--FILE--
<?php
function foo() {
    echo "foo()\n";
}

function bar() {
    return 3;
}

function test($value) {
    var_dump(match ($value) {
        1 => { 1 },
        2 => {
            $x = 2;
            $x
        },
        3 => {
            foo();
            bar()
        },
    });
}

test(1);
test(2);
test(3);
?>
--EXPECT--
int(1)
int(2)
foo()
int(3)
