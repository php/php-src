--TEST--
Returning array in output handler suppresses warning
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return ['some', 'array'];
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
ArrayAfter flush
