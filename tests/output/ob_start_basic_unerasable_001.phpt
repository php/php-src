--TEST--
ob_start(): Ensure content of unerasable buffer can be accessed by ob_get_contents().
--FILE--
<?php
function callback($string) {
	static $callback_invocations;
	$callback_invocations++;
	return "[callback:$callback_invocations]$string\n";
}

ob_start('callback', 0, false);

echo "This call will obtain the content:\n";
$str = ob_get_contents();
var_dump($str);
?>
==DONE==
--EXPECTF--
[callback:1]This call will obtain the content:
string(35) "This call will obtain the content:
"
==DONE==
