--TEST--
Variance: covariant T forwarded into a contravariant `implements` arg is rejected at link time when the target autoloads later
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    static $sources = [
        'AUI2_Sink' => 'interface AUI2_Sink<in U> {}',
        'AUI2_Bad' => 'final class AUI2_Bad<out T> implements AUI2_Sink<T> {}',
    ];

    if (isset($sources[$class])) {
        eval($sources[$class]);
    }
});

class_exists('AUI2_Bad');
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
