--TEST--
class constants as default function arguments
--FILE--
<?php

class test {
    const val = 1;
}

function foo($v = test::val) {
    var_dump($v);
}

function bar($b = NoSuchClass::val) {
    var_dump($b);
}

foo();
foo(5);

bar(10);
bar();

echo "Done\n";
?>
--EXPECTF--
int(1)
int(5)
int(10)

Fatal error: Uncaught Error: Class "NoSuchClass" not found in %s:%d
Stack trace:
#0 %s(%d): bar()
#1 {main}
  thrown in %s on line %d
