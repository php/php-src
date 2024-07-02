--TEST--
Errors in output handler are suppressed
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    // Undefined variable
    return $undef;
}


var_dump(ob_start('handler'));
echo "done";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
After flush
