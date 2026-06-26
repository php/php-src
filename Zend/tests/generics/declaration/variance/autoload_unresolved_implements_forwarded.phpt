--TEST--
Variance: covariant T forwarded into an `implements` arg whose target autoloads later is accepted
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    static $sources = [
        'AUI_Sequence' => 'interface AUI_Sequence<out T> {}',
        'AUI_ArraySequence' => 'final class AUI_ArraySequence<out T> implements AUI_Sequence<T> {}',
    ];

    if (isset($sources[$class])) {
        eval($sources[$class]);
    }
});

class_exists('AUI_ArraySequence');
echo "ok\n";
?>
--EXPECT--
ok
