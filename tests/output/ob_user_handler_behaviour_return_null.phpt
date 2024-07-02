--TEST--
Returning null in output handler is like returning empty string
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return null;
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
After flush
