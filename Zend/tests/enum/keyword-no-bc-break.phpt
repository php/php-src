--TEST--
Enum keyword can still be used in classes, namespaces, functions and constants
--FILE--
<?php

namespace enum {
    class Foo {}
}

namespace foo {
    class Bar {}
    class enum extends Bar {}
}

namespace bar {
    interface Baz {}
    class enum implements Baz {}
}

namespace {
    class enum {}

    function enum() {
        return 'enum function';
    }

    const enum = 'enum constant';

    var_dump(new enum\Foo());
    var_dump(new enum());
    var_dump(enum());
    var_dump(enum);
}

?>
--EXPECT--
object(enum\Foo)#1 (0) {
}
object(enum)#1 (0) {
}
string(13) "enum function"
string(13) "enum constant"
