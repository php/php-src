--TEST--
Testing error on null parameter 1 of http_build_query()
--CREDITS--
Pawel Krynicki <pawel.krynicki [at] xsolve [dot] pl>
#testfest AmsterdamPHP 2012-06-23
--FILE--
<?php

$result = http_build_query(null);

?>
--EXPECTF--
Warning: http_build_query() expects parameter 1 to be array, null given in %s on line %d
