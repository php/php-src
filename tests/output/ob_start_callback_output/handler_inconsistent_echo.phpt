--TEST--
ob_start(): Check behaviour with handler that doesn't always trigger output
--FILE--
<?php

$log = [];

function handler($string) {
    global $log;
    $log[] = __FUNCTION__ . ": <<<" . $string . ">>>";
    if ($string === "DO ECHO\n") {
        echo __FUNCTION__;
    }
    return $string;
}

ob_start('handler');
echo "DO ECHO\n";
ob_flush();
echo "NO ECHO\n";
ob_flush();
echo "DO ECHO\n";
ob_flush();
echo "LAST ONE\n";
ob_end_flush();

echo "\n\nLog:\n";
echo implode("\n", $log);

?>
--EXPECTF--
Deprecated: ob_flush(): Producing output from user output handler handler is deprecated in %s on line %d
DO ECHO
NO ECHO

Deprecated: ob_flush(): Producing output from user output handler handler is deprecated in %s on line %d
DO ECHO
LAST ONE


Log:
handler: <<<DO ECHO
>>>
handler: <<<NO ECHO
>>>
handler: <<<DO ECHO
>>>
handler: <<<LAST ONE
>>>
