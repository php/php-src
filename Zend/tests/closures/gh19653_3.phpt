--TEST--
GH-19653 (Closure named argument unpacking between temporary closures can cause a crash) - temporary method variation
--EXTENSIONS--
zend_test
--FILE--
<?php

function usage1($f) {
    $f(tmpMethodParamName: null);
}

usage1([new _ZendTestClass(), 'testTmpMethodWithArgInfo']);
usage1(eval('return function (string $a, string $b): string { return $a.$b; };'));

?>
--EXPECTF--
Fatal error: Uncaught Error: Unknown named parameter $tmpMethodParamName in %s:%d
Stack trace:
#0 %s(%d): usage1(Object(Closure))
#1 {main}
  thrown in %s on line %d
