--TEST--
Test # delimiter parsing and execution
--PHPDBG--
ev 1 + 3 # ev 2 ** 3#q
--EXPECTF--
prompt> 4
8
