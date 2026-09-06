--TEST--
Bug #31190 (exception in call_user_func_array())
--FILE--
<?php

class test {
     function throwException() { throw new Exception("Hello World!");
} }

$array = array(new test(), 'throwException');
try {
     call_user_func($array, 1, 2);
} catch (Throwable $e) {
     echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
     call_user_func_array($array, array(1, 2));
} catch (Throwable $e) {
     echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Hello World!
Exception: Hello World!
