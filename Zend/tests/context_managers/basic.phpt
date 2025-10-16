--TEST--
Context manager basic
--FILE--
<?php

function some_context() {
    yield 'hello';
}

using (some_context() => $var) {
    echo "Body is executed once\n";
    echo "\$var: ";
    var_dump($var);
};

echo "\$var is unset() after the block: ";
var_dump(!isset($var));

?>
--EXPECT--
Body is executed once
$var: string(5) "hello"
$var is unset() after the block: bool(true)
