--TEST--
ob_start(): Ensure unerasable buffer cannot be accessed or flushed by ob_get_flush().
--FILE--
<?php
function callback($string) {
    static $callback_invocations;
    $callback_invocations++;
    return "[callback:$callback_invocations]$string\n";
}

ob_start('callback', 0, false);

echo "This call will obtain the content, but will not flush the buffer.";
$str = ob_get_flush();
var_dump($str);
?>
--EXPECTF--
[callback:1]This call will obtain the content, but will not flush the buffer.
Notice: ob_get_flush(): Failed to send buffer of callback (0) in %s on line 11

Notice: ob_get_flush(): Failed to delete buffer of callback (0) in %s on line 11
string(65) "This call will obtain the content, but will not flush the buffer."
