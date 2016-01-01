--TEST--
Stepping with exceptions must not be stuck at CATCH
--INI--
opcache.enable=0
--PHPDBG--
b ZEND_THROW
r
s






q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at ZEND_THROW]
prompt> [Breakpoint #0 in ZEND_THROW at %s:4, hits: 1]
>00004: 	throw new Exception;
 00005: }
 00006: 
prompt> [L0 %s HANDLE_EXCEPTION                                                                       %s]
>00004: 	throw new Exception;
 00005: }
 00006: 
prompt> [L0 %s HANDLE_EXCEPTION                                                                       %s]
[L9 %s CATCH                   "Exception"          $e                   1                    %s]
>00008: 	foo();
 00009: } catch (Exception $e) {
 00010: 	echo "ok";
prompt> [L10 %s ECHO                    "ok"                                                           %s]
>00010: 	echo "ok";
 00011: } finally {
 00012: 	echo " ... ok";
prompt> ok
[L11 %s FAST_CALL               J8                                        ~%d                   %s]
>00011: } finally {
 00012: 	echo " ... ok";
 00013: }
prompt> [L12 %s ECHO                    " ... ok"                                                      %s]
>00012: 	echo " ... ok";
 00013: }
 00014: 
prompt>  ... ok
[L12 %s FAST_RET                ~%d                                                             %s]
[L11 %s JMP                     J10                                                            %s]
>00011: } finally {
 00012: 	echo " ... ok";
 00013: }
prompt> [L12 %s RETURN                  1                                                              %s]
>00012: 	echo " ... ok";
 00013: }
 00014: 
prompt> 
--FILE--
<?php

function foo() {
	throw new Exception;
}

try {
	foo();
} catch (Exception $e) {
	echo "ok";
} finally {
	echo " ... ok";
}
