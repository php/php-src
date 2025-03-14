--TEST--
#[\NoDiscard]: execute_internal overwritten
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
--FILE--
<?php

$f = tmpfile();
flock($f, LOCK_SH | LOCK_NB);
fclose($f);

?>
--EXPECTF--
<!-- internal enter tmpfile() -->
<!-- internal enter NoDiscard::__construct() -->

Warning: The return value of function flock() should either be used or intentionally ignored by casting it as (void), as locking the stream might have failed in %s on line %d
<!-- internal enter flock() -->
<!-- internal enter fclose() -->
