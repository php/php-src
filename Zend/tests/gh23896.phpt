--TEST--
GH-23896 (Assertion failure in zend_call_function() when the error handler throws during parent:: callable resolution)
--INI--
unserialize_callback_func=parent::my_unserialize
--FILE--
<?php
set_error_handler(function ($severity, $m) {
    throw new Exception($m, $severity);
});
class P {}
class C extends P {
    public function u(string $s) {
        return unserialize($s);
    }
}
try {
    (new C)->u('O:3:"FOO":0:{}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECT--
Use of "parent" in callables is deprecated
NULL
