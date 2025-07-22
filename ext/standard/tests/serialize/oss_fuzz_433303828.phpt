--TEST--
OSS-Fuzz #433303828
--FILE--
<?php

unserialize('O:2:"yy": ');
unserialize('O:2:"yy":: ');

?>
--EXPECTF--
Warning: unserialize(): Error at offset 9 of 10 bytes in %s on line %d

Warning: unserialize(): Error at offset 10 of 11 bytes in %s on line %d
