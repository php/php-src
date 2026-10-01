--TEST--
Bug #78926: Segmentation fault on Symfony cache:clear
--FILE--
<?php

spl_autoload_register(function($class) {
    for ($i = 0; $i < 100; $i++) {
        eval("class C$i {}");
    }
});

try {
    class B extends A {}
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(class_exists('B', false));

?>
--EXPECT--
Error: Class "A" not found
bool(false)
