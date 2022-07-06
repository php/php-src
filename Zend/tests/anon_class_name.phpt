--TEST--
Generated names for anonymous classes
--FILE--
<?php

namespace DeclaringNS {
    class Test1 {}
    interface Test2 {}
    interface Test3 {}
}

namespace UsingNS {
    function print_name(object $obj) {
        echo strstr(get_class($obj), "\0", true), "\n";
    }

    print_name(new class {});
    print_name(new class extends \DeclaringNS\Test1 {});
    print_name(new class extends \DeclaringNS\Test1 implements \DeclaringNS\Test2 {});
    print_name(new class implements \DeclaringNS\Test2 {});
    print_name(new class implements \DeclaringNS\Test2, \DeclaringNS\Test3 {});
}

?>
--EXPECT--
class@anonymous
DeclaringNS\Test1@anonymous
DeclaringNS\Test1@anonymous
DeclaringNS\Test2@anonymous
DeclaringNS\Test2@anonymous
