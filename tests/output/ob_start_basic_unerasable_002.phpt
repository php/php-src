--TEST--
ob_start(): Ensure unerasable buffer cannot be erased by ob_clean(), ob_end_clean() or ob_end_flush().
--FILE--
<?php
function callback($string) {
	static $callback_invocations;
	$callback_invocations++;
	return "[callback:$callback_invocations]$string\n";
}

ob_start('callback', 0, false);

echo "All of the following calls will fail to clean/remove the topmost buffer:\n";
var_dump(ob_clean());
var_dump(ob_end_clean());
var_dump(ob_end_flush());

echo "The OB nesting will still be 1 level deep:\n";
var_dump(ob_get_level());
?>
--EXPECTF--
[callback:1]All of the following calls will fail to clean/remove the topmost buffer:

Notice: ob_clean(): failed to delete buffer of callback (0) in %s on line 11
bool(false)

Notice: ob_end_clean(): failed to discard buffer of callback (0) in %s on line 12
bool(false)

Notice: ob_end_flush(): failed to send buffer of callback (0) in %s on line 13
bool(false)
The OB nesting will still be 1 level deep:
int(1)
