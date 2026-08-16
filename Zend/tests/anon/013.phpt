--TEST--
closure binding to anonymous class
--FILE--
<?php
$class = new class {};
$foo = function() {
    return $this;
};

$closure = Closure::bind($foo, $class, $class);
var_dump($closure());
?>
--EXPECTF--
object(class@anonymous%0%s:2$%x)#1 (0) {
}
