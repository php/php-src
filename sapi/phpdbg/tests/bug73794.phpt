--TEST--
Bug #73794 (Crash (out of memory) when using run and # command separator)
--PHPDBG--
r echo # quit
--EXPECTF--
[Successful compilation of %s]
prompt> echo
--FILE--
<?php
echo $argv[1];
?>
