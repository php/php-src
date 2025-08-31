--TEST--
get_object_vars() - ensure statics are not shown
--FILE--
<?php
Class A {
    public static $var = 'hello';
}

$a = new A;
var_dump(get_object_vars($a));
?>
--EXPECT--
array(0) {
}
