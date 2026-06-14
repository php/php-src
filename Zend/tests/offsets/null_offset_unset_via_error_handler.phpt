--TEST--
No UAF on null offset deprecation with unset value
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    var_dump($errstr);
    global $a;
    unset($a);
});

$a = new stdClass;
$b = [0, null => $a];

echo "\nSuccess\n";
?>
--EXPECTF--
string(72) "Using null as an array offset is deprecated, use an empty string instead"

Success
