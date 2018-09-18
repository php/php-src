--TEST--
string gethostname(void);
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
var_dump(gethostname());
var_dump(gethostname("php-zend-brazil"));
?>
--EXPECTF--
%s

Warning: gethostname() expects exactly %d parameters, %d given in %s on line %d
NULL
