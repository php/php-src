--TEST--
Testing parameter type-hinted with default value inside namespace
--FILE--
<?php

namespace foo;

class bar {
    public function __construct(?\stdClass $x = NULL) {
        var_dump($x);
    }
}

new bar(new \stdClass);
new bar(null);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
NULL
