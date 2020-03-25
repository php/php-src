--TEST--
Testing parameter type-hinted with interface
--FILE--
<?php

namespace foo;

interface foo {

}

class bar {
    public function __construct(foo $x = NULL) {
        var_dump($x);
    }
}

class test implements foo {

}


new bar(new test);
new bar(null);
new bar(new \stdclass);

?>
--EXPECTF--
object(foo\test)#%d (0) {
}
NULL

Fatal error: Uncaught TypeError: foo\bar::__construct(): Argument #1 ($x) must be of type ?foo\foo, stdClass given, called in %s:%d
Stack trace:
#0 %s(%d): foo\bar->__construct(Object(stdClass))
#1 {main}
  thrown in %s on line %d
