--TEST--
ob_start(): Check behaviour with multiple nested handlers with output
--FILE--
<?php

$log = [];

function first_handler($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    echo __FUNCTION__;
    return "FIRST\n";
}

function second_handler($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    echo __FUNCTION__;
    return "SECOND\n";
}

function third_handler($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    echo __FUNCTION__;
    return "THIRD\n";
}

ob_start('first_handler');
ob_start('second_handler');
ob_start('third_handler');

echo "Testing...";

ob_end_flush();
ob_end_flush();
ob_end_flush();

echo "\n\nLog:\n";
echo implode("\n", $log);
?>
--EXPECTF--
Deprecated: ob_end_flush(): Producing output from user output handler first_handler is deprecated in %s on line %d
FIRST


Log:
third_handler: <<<Testing...>>>
second_handler: <<<
Deprecated: ob_end_flush(): Producing output from user output handler third_handler is deprecated in %s on line %d
THIRD
>>>
first_handler: <<<
Deprecated: ob_end_flush(): Producing output from user output handler second_handler is deprecated in %s on line %d
SECOND
>>>
