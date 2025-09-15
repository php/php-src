--TEST--
No UAF on null offset
--FILE--
<?php
set_error_handler(function($errno, $errstr) {
    global $ary;
    $ary = null;
    echo $errstr;
});

$ary[null] = 1;

echo "\nSuccess\n";
?>
--EXPECTF--
Using null as an array offset is deprecated, use an empty string instead
Success
