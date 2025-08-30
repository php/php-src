--TEST--
FCC in const expression triggers autoloader.
--FILE--
<?php

spl_autoload_register(static function ($class) {
    echo "Autoloading {$class}", PHP_EOL;
    eval(
        <<<'EOT'
            class AutoloadedClass {
                public static function withStaticMethod() {
                    echo "Called ", __METHOD__, PHP_EOL;
                }
            }
            EOT
    );
});

const Closure = AutoloadedClass::withStaticMethod(...);

var_dump(Closure);
(Closure)();

?>
--EXPECTF--
Autoloading AutoloadedClass
object(Closure)#%d (1) {
  ["function"]=>
  string(33) "AutoloadedClass::withStaticMethod"
}
Called AutoloadedClass::withStaticMethod
