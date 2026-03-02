--TEST--
ob_start(): Check behaviour with deprecation converted to exception [produce output]
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

$cases = [
    'first_handler',
    'second_handler',
    'third_handler',
];
foreach ($cases as $case) {
    $log = [];
    echo "\n\nTesting: $case\n";
    ob_start($case);
    echo "Inside of $case\n";
    try {
        ob_end_flush();
    } catch (\ErrorException $e) {
        echo $e . "\n";
    }
    echo "\nEnd of $case, log was:\n";
    echo implode("\n", $log);
}

?>
--EXPECTF--
Testing: first_handler
FIRST
ErrorException: ob_end_flush(): Producing output from user output handler first_handler is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, 41)
#1 %s(%d): ob_end_flush()
#2 {main}

End of first_handler, log was:
first_handler: <<<Inside of first_handler
>>>

Testing: second_handler
SECOND
ErrorException: ob_end_flush(): Producing output from user output handler second_handler is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, 41)
#1 %s(%d): ob_end_flush()
#2 {main}

End of second_handler, log was:
second_handler: <<<Inside of second_handler
>>>

Testing: third_handler
THIRD
ErrorException: ob_end_flush(): Producing output from user output handler third_handler is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, 41)
#1 %s(%d): ob_end_flush()
#2 {main}

End of third_handler, log was:
third_handler: <<<Inside of third_handler
>>>
