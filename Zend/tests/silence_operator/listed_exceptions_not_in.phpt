--TEST--
Suppression operator should only suppress listed Exceptions
--FILE--
<?php

function test1() {
    throw new Error();
    return true;
}

$var = @<Exception>test1();

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error in %s:4
Stack trace:
#0 %s(8): test1()
#1 {main}
  thrown in %s on line 4
