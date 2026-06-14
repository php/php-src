--TEST--
ob_start(): Check behaviour with nested deprecation converted to exception [produce output]
--FILE--
<?php

$log = [];

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

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

echo "In all of them\n\n";
try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended third_handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended second_handler\n\n";

try {
    ob_end_flush();
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}
echo "Ended first_handler handler\n\n";

echo "All handlers are over\n\n";
echo implode("\n", $log);

?>
--EXPECT--
FIRST
ob_end_flush(): Producing output from user output handler first_handler is deprecated
Ended first_handler handler

All handlers are over

third_handler: <<<In all of them

>>>
second_handler: <<<THIRD
ob_end_flush(): Producing output from user output handler third_handler is deprecated
Ended third_handler

>>>
first_handler: <<<SECOND
ob_end_flush(): Producing output from user output handler second_handler is deprecated
Ended second_handler

>>>
