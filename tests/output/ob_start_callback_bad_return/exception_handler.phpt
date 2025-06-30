--TEST--
ob_start(): Check behaviour with deprecation converted to exception
--FILE--
<?php

$log = [];

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

function return_null($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return null;
}

function return_false($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return false;
}

function return_true($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return true;
}

function return_zero($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    return 0;
}

$cases = ['return_null', 'return_false', 'return_true', 'return_zero'];
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
Testing: return_null
ErrorException: ob_end_flush(): Returning a non-string result from user output handler return_null is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, %d)
#1 %s(%d): ob_end_flush()
#2 {main}

End of return_null, log was:
return_null: <<<Inside of return_null
>>>

Testing: return_false
Inside of return_false
ErrorException: ob_end_flush(): Returning a non-string result from user output handler return_false is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, %d)
#1 %s(%d): ob_end_flush()
#2 {main}

End of return_false, log was:
return_false: <<<Inside of return_false
>>>

Testing: return_true
ErrorException: ob_end_flush(): Returning a non-string result from user output handler return_true is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, %d)
#1 %s(%d): ob_end_flush()
#2 {main}

End of return_true, log was:
return_true: <<<Inside of return_true
>>>

Testing: return_zero
0ErrorException: ob_end_flush(): Returning a non-string result from user output handler return_zero is deprecated in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'ob_end_flush():...', %s, %d)
#1 %s(%d): ob_end_flush()
#2 {main}

End of return_zero, log was:
return_zero: <<<Inside of return_zero
>>>
