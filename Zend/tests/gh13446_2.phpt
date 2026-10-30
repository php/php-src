--TEST--
GH-13446: Exception handler attempting to free itself
--FILE--
<?php
$x = new \stdClass();
$handler = function ($ex) use (&$handler, $x) {
    $handler = null;
    var_dump($x);
};
unset($x);
set_exception_handler($handler);
throw new Exception('Unhandled');
?>
--EXPECT--
object(stdClass)#1 (0) {
}
