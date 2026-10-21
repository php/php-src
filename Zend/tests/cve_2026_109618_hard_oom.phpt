--TEST--
CVE-2026-109618 OOM terminates without running request cleanup callbacks
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
	die("skip Zend MM disabled");
}
?>
--FILE--
<?php
register_shutdown_function(function () {
	echo "UNSAFE SHUTDOWN\n";
});
$buffer = str_repeat("x", 32 * 1024 * 1024);
echo "UNREACHABLE\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%S(tried to allocate %d bytes) in %s on line %d
