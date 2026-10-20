--TEST--
Static variables in dynamically declared function (first use before dynamic def dtor)
--FILE--
<?php

$code = <<<'CODE'
if (1) {
    function test() {
        static $x = 0;
        var_dump(++$x);
    }
    test();
}
CODE;
eval($code);
test();

?>
--EXPECT--
int(1)
int(2)
