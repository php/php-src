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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(class_exists('B', false));

?>
--EXPECT--
Class "A" not found
bool(false)
