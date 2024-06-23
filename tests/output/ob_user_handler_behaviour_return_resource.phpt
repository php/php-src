--TEST--
Returning resource in output handler suppresses warning
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return STDERR;
}


var_dump(ob_start('handler'));
echo "done";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
Resource id #3After flush
