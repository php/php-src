--TEST--
GH-24492: vprintf allocation failure must not reach the hard memory limit
--INI--
memory_limit=16M
memory_hard_limit_percent=200
--FILE--
<?php
register_shutdown_function(static function () { echo "shutdown\n"; });
vprintf("%40000000s", [""]);
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
shutdown
