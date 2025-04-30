--TEST--
Properly handle exceptions going to be uncaught
--PHPDBG--
r
t
ev 1 + 2
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> handle first
[Uncaught Error in %s on line 16: Call to undefined function foo()]
>00016: 		foo(); // Error
 00017: 	} catch (\Exception $e) {
 00018: 		var_dump($e);
prompt> frame #0: {closure:%s:%d}() at %s:16
frame #1: {main} at %s:22
prompt> 3
prompt> [Uncaught Error in %s on line 16]
Error: Call to undefined function foo() in %s:16
Stack trace:
#0 %s(22): {closure:%s:%d}()
#1 {main}
[Script ended normally]
prompt> 
--FILE--
<?php

(function() {
	try {
		foo(); // Error
	} catch (\Exception $e) {
		var_dump($e);
	} finally {
		print "handle first\n";
		return "ok";
	}
})();

(function() {
	try {
		foo(); // Error
	} catch (\Exception $e) {
		var_dump($e);
	} catch (\ParseError $e) {
		var_dump($e);
	}
})();
