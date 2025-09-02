--TEST--
GH-16509: Incorrect lineno reported for function redeclaration
--FILE--
<?php

include __DIR__ . '/gh16509.inc';
include __DIR__ . '/gh16509.inc';

?>
--EXPECTF--
Fatal error: Cannot redeclare function test() (previously declared in %sgh16509.inc:3) in %sgh16509.inc on line 3
