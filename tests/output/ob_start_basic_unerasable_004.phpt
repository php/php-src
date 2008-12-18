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

echo "This call will fail to flush and fail to obtain the content:\n";
$str = ob_get_flush();
var_dump($str);
?>
--EXPECTF--
[callback:1]This call will fail to flush and fail to obtain the content:

Notice: ob_get_flush(): failed to delete buffer callback. in %s on line 11
bool(false)
