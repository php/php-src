--TEST--
Returning stringable object in output handler
--FILE--
<?php

class A {
    public function __toString() { return "Object content\n"; }
}

function handler(string $buffer, int $phase) {
    echo "In handler";
    return new A();;
}

var_dump(ob_start('handler'));
echo "done\n";

ob_end_flush();

echo "After flush\n";

?>
--EXPECT--
Object content
After flush
