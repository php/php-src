--TEST--
Throwing exception in output handler will display output produced in the handler
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    throw new Exception("Handler is broken");
}


var_dump(ob_start('handler'));
echo "done";

ob_end_flush();

echo "After flush\n";

?>
--EXPECTF--
bool(true)
doneIn handler
Fatal error: Uncaught Exception: Handler is broken in %s:%d
Stack trace:
#0 [internal function]: handler('bool(true)\ndone', 9)
#1 %s(12): ob_end_flush()
#2 {main}
  thrown in %s on line %d
