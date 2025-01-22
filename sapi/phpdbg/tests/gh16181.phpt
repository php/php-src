--TEST--
GH-16181 (phpdbg: exit in exception handler reports fatal error)
--PHPDBG--
r
c
q
--FILE--
<?php
set_exception_handler(function() {
	echo "exception caught\n";
	die;
});

echo "throwing exception\n";
throw new \Exception("oh noes");
?>
--EXPECTF--
[Successful compilation of %s]
prompt> throwing exception
[Uncaught Exception in %s on line %d: oh noes]
>00008: throw new \Exception("oh noes");
 00009: ?>
 00010: 
prompt> exception caught
[Script ended normally]
prompt>
