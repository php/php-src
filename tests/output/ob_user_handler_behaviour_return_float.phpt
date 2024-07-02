--TEST--
Returning float in output handler
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return 24.74;
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
24.74After flush
