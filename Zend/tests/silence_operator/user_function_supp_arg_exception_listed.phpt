--TEST--
Listed silencing a userland function supplementary argument throwing an Exception
--FILE--
<?php

function test1() {
    return func_get_args();
}

function foo() {
    throw new Exception();
    return 1;
}

$var = test1(@<Exception>foo());

var_dump($var);

echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
Done
