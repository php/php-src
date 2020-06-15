--TEST--
Can declare classes with reserved name
--FILE--
<?php

namespace FooBar {
    class List {}
    function list() { return []; }
    const LIST = [];

    class Test {}
}

namespace BarFoo {
    use FooBar\List as MyList;
    use function FooBar\list as get_list;
    use const FooBar\LIST as THE_LIST;
    use /* namespace */ FooBar as List;

    var_dump(new \FooBar\List);
    var_dump(new MyList);

    var_dump(\FooBar\list());
    var_dump(get_list());

    var_dump(\FooBar\LIST);
    var_dump(THE_LIST);

    var_dump(new List\Test);
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
object(FooBar\Test)#1 (0) {
}
