--TEST--
Testing parameter type-hinted (array) with default value inside namespace
--FILE--
<?php

namespace foo;

class bar {
    public function __construct(array $x = NULL) {
        var_dump($x);
    }
}

new bar(null);
new bar(new \stdclass);

?>
--EXPECTF--
NULL

Fatal error: Uncaught TypeError: foo\bar::__construct(): Argument #1 ($x) must be of type ?array, stdClass given, called in %s:%d
Stack trace:
#0 %s(%d): foo\bar->__construct(Object(stdClass))
#1 {main}
  thrown in %s on line %d
