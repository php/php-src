--TEST--
Unset declared property converted to object in error handler
--FILE--
<?php
class C {
    public $a;
    function errorHandler() {
        $this->a = new stdClass();
    }
}
$c = new C;
set_error_handler([$c,'errorHandler']);
unset($c->a);

try {
    (++$c->a);
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
var_dump($c->a);
?>
--EXPECT--
Cannot increment stdClass
object(stdClass)#2 (0) {
}
