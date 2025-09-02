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

ob_start('handle_echo');
ob_start('handle_var_dump');
ob_start('handle_var_export');
ob_start('handle_phpcredits');

echo "Testing...";

ob_end_flush();
ob_end_flush();
ob_end_flush();
ob_end_flush();

echo "\n\nLog:\n";
echo implode("\n", $log);
?>
--EXPECTF--
Deprecated: ob_end_flush(): Producing output from user output handler handle_echo is deprecated in %s on line %d
echo


Log:
handle_phpcredits: <<<Testing...>>>
handle_var_export: <<<
Deprecated: ob_end_flush(): Producing output from user output handler handle_phpcredits is deprecated in %s on line %d
phpcredits
>>>
handle_var_dump: <<<
Deprecated: ob_end_flush(): Producing output from user output handler handle_var_export is deprecated in %s on line %d
var_export
>>>
handle_echo: <<<
Deprecated: ob_end_flush(): Producing output from user output handler handle_var_dump is deprecated in %s on line %d
var_dump
>>>