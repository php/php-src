--TEST--
registering a function and calling it leaks arguments memory
--FILE--
<?php
echo "Done\n";
?>
--PHPDBG--
register var_dump
var_dump "a" "b"
register flush
flush
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Registered var_dump]
prompt> string(1) "a"
string(1) "b"

prompt> [Registered flush]
prompt> 
prompt> Done
[Script ended normally]
prompt>
