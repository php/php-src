--TEST--
Test exceptions in eval during exception
--PHPDBG--
r
ev next_error()
c

q
--EXPECTF--
[Successful compilation of %s]
prompt> handle first
[Uncaught Error in %s on line 16: Call to undefined function foo()]
>00016: 		foo(); // Error
 00017: 	} catch (\Exception $e) {
 00018: 		var_dump($e);
prompt> 
Fatal error: Uncaught Error: Call to undefined function next_error() in eval()'d code:1
Stack trace:
#0 %s(16): unknown()
#1 %s(20): {closure}()
#2 {main}
  thrown in eval()'d code on line 1
prompt> [Uncaught Error in %s on line 16]
Error: Call to undefined function foo() in %s:16
Stack trace:
#0 %s(20): {closure}()
#1 {main}
[Script ended normally]
prompt> [The stack contains nothing !]
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
	}
})();
