--TEST--
GH-23896 (Assertion failure in zend_call_function() when the error handler throws during parent:: callable resolution)
--FILE--
<?php
set_error_handler(function ($severity, $m) {
    throw new Exception($m, $severity);
});
spl_autoload_register(function ($class) {
    if ($class === 'Loader') {
        throw new Exception("Cannot load $class");
    }
});
class P {}
class C extends P {
    public function u(string $s) {
        return unserialize($s);
    }
}
foreach (['parent::my_unserialize', 'Loader::load'] as $callback) {
    ini_set('unserialize_callback_func', $callback);
    try {
        (new C)->u('O:3:"FOO":0:{}');
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
        var_dump($e->getPrevious());
    }
}
?>
--EXPECT--
Use of "parent" in callables is deprecated
NULL
Cannot load Loader
NULL
