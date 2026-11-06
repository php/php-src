--TEST--
GH-24492: pack_format allocation failure must not reach the hard memory limit
--INI--
memory_limit=16M
memory_hard_limit_percent=200
--FILE--
<?php
register_shutdown_function(static function () { echo "shutdown\n"; });
$format = str_repeat("x", 8 * 1024 * 1024);
pack($format);
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line 4
shutdown
