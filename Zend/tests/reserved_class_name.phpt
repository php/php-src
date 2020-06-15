--TEST--
Can declare classes with reserved name
--FILE--
<?php

namespace FooBar {
    class List {}
}

namespace BarFoo {
    use FooBar\List as TheList;

    var_dump(new \FooBar\List);
    var_dump(new TheList);
}

?>
--EXPECT--
object(FooBar\List)#1 (0) {
}
object(FooBar\List)#1 (0) {
}
