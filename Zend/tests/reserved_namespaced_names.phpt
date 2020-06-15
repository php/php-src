--TEST--
Can declare classes with reserved name
--FILE--
<?php

namespace FooBar {
    class List {}
    function list() { return []; }
    const LIST = [];
}

namespace BarFoo {
    use FooBar\List as MyList;
    use function FooBar\list as get_list;
    use const FooBar\LIST as THE_LIST;

    var_dump(new \FooBar\List);
    var_dump(new MyList);

    var_dump(\FooBar\list());
    var_dump(get_list());

    var_dump(\FooBar\LIST);
    var_dump(THE_LIST);
}

?>
--EXPECT--
object(FooBar\List)#1 (0) {
}
object(FooBar\List)#1 (0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
