--TEST--
Bug #20242 (Method call in front of class definition)
--FILE--
<?php
test::show_static();

$t = new test;
$t->show_method();

class test {
    static function show_static() {
        echo "static\n";
    }
    function show_method() {
        echo "method\n";
    }
}
?>
--EXPECT--
static
method
