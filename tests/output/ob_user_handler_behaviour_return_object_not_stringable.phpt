--TEST--
Returning non stringable object in output handler
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return new stdClass();
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class stdClass could not be converted to string in %s:%d
Stack trace:
#0 %s(%d): ob_end_flush()
#1 {main}
  thrown in %s on line %d
