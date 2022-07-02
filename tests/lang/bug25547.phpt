--TEST--
Bug #25547 (error_handler and array index with function call)
--FILE--
<?php

function handler($errno, $errstr, $errfile, $errline)
{
    echo __FUNCTION__ . "($errstr)\n";
}

set_error_handler('handler');

function foo($x) {
    return "foo";
}

$output = array();
++$output[foo("bar")];

print_r($output);

echo "Done";
?>
--EXPECT--
handler(Undefined array key "foo")
Array
(
    [foo] => 1
)
Done
