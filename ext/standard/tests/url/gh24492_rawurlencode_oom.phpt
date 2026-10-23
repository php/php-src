--TEST--
GH-24492: rawurlencode() reports OOM without skipping shutdown functions
--INI--
memory_limit=16M
memory_hard_limit_percent=200
fatal_error_backtraces=0
--FILE--
<?php
register_shutdown_function(static function () {
    echo "shutdown\n";
});
$input = str_repeat("\0", 12 * 1024 * 1024);
echo "input ready\n";
rawurlencode($input);
echo "not reached\n";
?>
--EXPECTF--
input ready

Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
shutdown
