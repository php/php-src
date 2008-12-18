--TEST--
ob_start(): Ensure unerasable buffer cannot be accessed or erased by ob_get_clean().
--FILE--
<?php
function callback($string) {
	static $callback_invocations;
	$callback_invocations++;
	return "[callback:$callback_invocations]$string\n";
}

ob_start('callback', 0, false);

echo "This call will fail to obtain the content, since it is also requesting a clean:\n";
$str = ob_get_clean();
var_dump($str);
?>
--EXPECTF--
[callback:1]This call will fail to obtain the content, since it is also requesting a clean:

Notice: ob_get_clean(): failed to delete buffer callback. in %s on line 11
bool(false)