--TEST--
Autoloading must not invalidate a string callable during a dynamic call
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    $GLOBALS['callable'][strlen($GLOBALS['callable']) - 1] = 'o';
    eval('class DynamicStringCallableTest {
        public static function mOne(): void { echo "mOne\\n"; }
        public static function mOno(): void { echo "mOno\\n"; }
    }');
});

// Keep this non-interned and uniquely owned so the offset write is in-place.
$callable = strrev('enOm::tseTelballaCgnirtScimanyD');
$callable();
?>
--EXPECT--
mOne
