--TEST--
Ensure callback methods in unknown classes trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
    echo "In autoload: ";
    var_dump($name);
});
try {
    call_user_func("UndefC::test");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
In autoload: string(6) "UndefC"
call_user_func(): Argument #1 ($callback) must be a valid callback, class "UndefC" not found
