--TEST--
Bug #47851 (is_callable throws fatal error)
--FILE--
<?php
class foo {
    function bar() {
        echo "ok\n";
    }
}
var_dump(is_callable(array('foo','bar')));
try {
    foo::bar();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(is_callable(array('Exception','getMessage')));
try {
    Exception::getMessage();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
Error: Non-static method foo::bar() cannot be called statically
bool(false)
Error: Non-static method Exception::getMessage() cannot be called statically
