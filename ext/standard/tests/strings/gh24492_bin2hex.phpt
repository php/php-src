--TEST--
GH-24492: bin2hex allocation failure must not reach the hard memory limit
--INI--
memory_limit=16M
memory_hard_limit_percent=200
--FILE--
<?php
register_shutdown_function(static function () { echo "shutdown\n"; });
$s = str_repeat("a", 10 * 1024 * 1024);
bin2hex($s);
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line 4
shutdown
