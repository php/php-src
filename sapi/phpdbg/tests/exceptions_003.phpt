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
prompt> [L0 %s HANDLE_EXCEPTION                                                                       %s]
>00005: 		x();
 00006: 	} finally {
 00007: 		print "ok\n";
prompt> [L7 %s ECHO                    "ok "                                                          %s]
>00007: 		print "ok\n";
 00008: 	}
 00009: } catch (Error $e) {
prompt> ok
[L7 %s FAST_RET<TO_CATCH>      ~%d                   J7                                        %s]
[L9 %s CATCH                   "Error"              $e                   1                    %s]
>00005: 		x();
 00006: 	} finally {
 00007: 		print "ok\n";
prompt> [L10 %s ECHO                    "caught "                                                      %s]
>00010: 	print "caught\n";
 00011: }
 00012: 
prompt> caught
[L10 %s RETURN                  1                                                              %s]
[Script ended normally]
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
