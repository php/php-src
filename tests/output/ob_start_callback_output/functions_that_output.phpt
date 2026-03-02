--TEST--
ob_start(): Check behaviour with functions that trigger output (nested)
--FILE--
<?php

$log = [];

function handle_echo($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    echo __FUNCTION__;
    return "echo\n";
}

function handle_var_dump($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    var_dump(__FUNCTION__);
    return "var_dump\n";
}

function handle_var_export($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    var_export(__FUNCTION__);
    return "var_export\n";
}

function handle_phpcredits($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    phpcredits();
    return "phpcredits\n";
}

$cases = [
    'handle_echo',
    'handle_var_dump',
    'handle_var_export',
    'handle_phpcredits',
];
foreach ($cases as $case) {
    $log = [];
    echo "\n\nTesting: $case";
    ob_start($case);
    echo "Inside of $case\n";
    ob_end_flush();
    echo "\nEnd of $case, log was:\n";
    echo implode("\n", $log);
}

?>
--EXPECTF--
Testing: handle_echo
Deprecated: ob_end_flush(): Producing output from user output handler handle_echo is deprecated in %s on line %d
echo

End of handle_echo, log was:
handle_echo: <<<Inside of handle_echo
>>>

Testing: handle_var_dump
Deprecated: ob_end_flush(): Producing output from user output handler handle_var_dump is deprecated in %s on line %d
var_dump

End of handle_var_dump, log was:
handle_var_dump: <<<Inside of handle_var_dump
>>>

Testing: handle_var_export
Deprecated: ob_end_flush(): Producing output from user output handler handle_var_export is deprecated in %s on line %d
var_export

End of handle_var_export, log was:
handle_var_export: <<<Inside of handle_var_export
>>>

Testing: handle_phpcredits
Deprecated: ob_end_flush(): Producing output from user output handler handle_phpcredits is deprecated in %s on line %d
phpcredits

End of handle_phpcredits, log was:
handle_phpcredits: <<<Inside of handle_phpcredits
>>>
