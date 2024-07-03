--TEST--
Returning true in output handler is like returning empty string
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return true;
}


var_dump(ob_start('handler'));
echo "done";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
After flush
