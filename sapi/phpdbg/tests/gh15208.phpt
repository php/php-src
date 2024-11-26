--TEST--
GH-15208 (Segfault with breakpoint map and phpdbg_clear())
--PHPDBG--
r
q
--FILE--
<?php
phpdbg_break_method("foo", "bar");
phpdbg_clear();
?>
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at foo::bar]
[Script ended normally]
prompt>
