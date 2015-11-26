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
Warning: http_build_query(): Parameter 1 expected to be Array or Object.  %s value given in %s on line %d