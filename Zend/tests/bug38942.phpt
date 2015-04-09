--TEST--
Bug #38942 (Double old-style-ctor inheritance)
--FILE--
<?php
class foo {
    public function foo() {}
}

class bar extends foo {
}
print_r(get_class_methods("bar"));
?>
--EXPECT--
Array
(
    [0] => foo
)
