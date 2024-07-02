--TEST--
Behaviour of user handlers when returning a string
--FILE--
<?php

function handler(string $buffer, int $phase) {
    echo "In handler";
    return $buffer;
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
bool(true)
done
After flush
