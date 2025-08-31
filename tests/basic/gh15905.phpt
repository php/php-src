--TEST--
GH-15905 (Assertion failure for TRACK_VARS_SERVER)
--INI--
variables_order=E
auto_globals_jit=0
register_argc_argv=1
--FILE--
<?php
echo "okay\n";
?>
--EXPECT--
okay
