--TEST--
GH-24492: base64_encode() uses soft memory limit without hard-limit bailout
--INI--
memory_limit=16M
memory_hard_limit_percent=200
--FILE--
<?php
register_shutdown_function(static function (): void { echo "FB_SHUTDOWN\n"; });
$input = str_repeat('A', 13 * 1024 * 1024);
echo "FB_INPUT_READY\n";
base64_encode($input);
echo "UNREACHABLE\n";
?>
--EXPECTF--
FB_INPUT_READY

Fatal error: Allowed memory size of 16777216 bytes exhausted%s
FB_SHUTDOWN
