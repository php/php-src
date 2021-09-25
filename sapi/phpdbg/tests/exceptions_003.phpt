--TEST--
Test breaks on HANDLE_EXCEPTION
--PHPDBG--
b 5
r
s

s


q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:5]
prompt> [Breakpoint #0 at %s:5, hits: 1]
>00005: 		x();
 00006: 	} finally {
 00007: 		print "ok\n";
prompt> >00005: 		x();
 00006: 	} finally {
 00007: 		print "ok\n";
prompt> >00007: 		print "ok\n";
 00008: 	}
 00009: } catch (Error $e) {
prompt> ok
>00005: 		x();
 00006: 	} finally {
 00007: 		print "ok\n";
prompt> >00010: 	print "caught\n";
 00011: }
 00012: 
prompt> caught
>00014: 
prompt>
--FILE--
<?php

try {
	try {
		x();
	} finally {
		print "ok\n";
	}
} catch (Error $e) {
	print "caught\n";
}

?>
