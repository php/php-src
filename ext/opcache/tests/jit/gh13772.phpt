--TEST--
EX(opline) is correctly set for nested JIT user code calls
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_output=0
--FILE--
<?php

function Ack($m, $n) {
	if ($m == 0) return $n+1;
	if ($n == 0) return Ack($m-1, 1);
	return Ack($m - 1, Ack($m, ($n - 1)));
}

var_dump(Ack(3, 3));

?>
--EXPECT--
int(61)

