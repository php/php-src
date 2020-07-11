--TEST--
string gethostname(void);
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
var_dump(gethostname());
?>
--EXPECTF--
string(%d) "%s"
