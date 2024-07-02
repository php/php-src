--TEST--
Returning false in output handler is like returning the $buffer string + any output produced in the handler
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return false;
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
bool(true)
done
In handlerAfter flush
