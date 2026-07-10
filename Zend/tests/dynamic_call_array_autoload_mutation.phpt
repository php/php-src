--TEST--
Autoloading must not invalidate an array callable during a dynamic call
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    $GLOBALS['method'] = 'changed';
    eval('class DynamicArrayCallableTest {
        public static function original(): void { echo "original\\n"; }
        public static function changed(): void { echo "changed\\n"; }
    }');
});

$method = 'original';
$callable = ['DynamicArrayCallableTest', &$method];
$callable();
?>
--EXPECT--
original
