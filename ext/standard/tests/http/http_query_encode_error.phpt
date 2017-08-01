--TEST--
Testing error on null parameter 1 of http_query_encode()
--CREDITS--
Pawel Krynicki <pawel.krynicki [at] xsolve [dot] pl>
#testfest AmsterdamPHP 2012-06-23
--FILE--
<?php

$result = http_query_encode(null);

?>
--EXPECTF--
Warning: http_query_encode(): Parameter 1 expected to be Array or Object. %s value given in %s on line %d
