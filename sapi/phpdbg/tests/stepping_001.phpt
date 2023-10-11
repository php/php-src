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
prompt> >00004: 	throw new Exception;
 00005: }
 00006: 
prompt> >00008: 	foo();
 00009: } catch (Exception $e) {
 00010: 	echo "ok\n";
prompt> >00010: 	echo "ok\n";
 00011: } finally {
 00012: 	echo " ... ok\n";
prompt> ok
>00011: } finally {
 00012: 	echo " ... ok\n";
 00013: }
prompt> >00012: 	echo " ... ok\n";
 00013: }
 00014: 
prompt>  ... ok
>00011: } finally {
 00012: 	echo " ... ok\n";
 00013: }
prompt> >00014: 
prompt>
--FILE--
<?php

function foo() {
	throw new Exception;
}

try {
	foo();
} catch (Exception $e) {
	echo "ok\n";
} finally {
	echo " ... ok\n";
}
?>
