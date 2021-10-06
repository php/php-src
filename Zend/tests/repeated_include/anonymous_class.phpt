--TEST--
Repeated include must not increase memory - anonymous class
--SKIPIF--
<?php
// TODO test should pass even without opcache, but fixes are needed
// related bug tracker https://bugs.php.net/bug.php?id=76982
if (!extension_loaded('Zend OPcache') || !(opcache_get_status() ?: ['opcache_enabled' => false])['opcache_enabled']) {
    die('xfail test currently requires opcache enabled');
}
?>
--FILE--
<?php

// new anonymous class declared and included multiple times always has a new class name,
// this test accounts for that and tests if an anonymous class declared in a closure
// invoked once does not increase memory consumption

final class AnonymousClassNameCache
{
    /** @var array<string, string> */
    private static $classNameByFxHash = [];

    private function __construct()
    {
    }

    public static function get_class(\Closure $createAnonymousClassFx): string
    {
        $fxRefl = new \ReflectionFunction($createAnonymousClassFx);
        $fxHash = $fxRefl->getFileName() . ':' . $fxRefl->getStartLine() . '-' . $fxRefl->getEndLine();

        if (!isset(self::$classNameByFxHash[$fxHash])) {
            self::$classNameByFxHash[$fxHash] = get_class($createAnonymousClassFx());
        }

        return self::$classNameByFxHash[$fxHash];
    }
}

$classNamePrev = null;
$m = -1;
$mDiff = -1;
$mPrev = 0;
for ($i = 0; $i < (PHP_OS_FAMILY === 'Windows' ? 1_000 /* include is slow on Windows */ : 20_000); $i++) {
    require __DIR__ . '/anonymous_class.inc';
    if ($classNamePrev !== null && $className !== $classNamePrev) {
        echo 'Class name is different: ' . $className . ' vs ' . $classNamePrev . ' (' . $i . ' iteration)' . "\n";
        exit(1);
    }
    $classNamePrev = $className;
    assert($className::verify() === 'ok');
    $m = memory_get_usage();
    $mDiff = $m - $mPrev;
    if ($mPrev !== 0 && $mDiff !== 0) {
        echo 'Increased memory detected: ' . $mDiff . ' B (' . $i . ' iteration)' . "\n";
        exit(1);
    }
    $mPrev = $m;
}
echo 'done';

?>
--EXPECT--
done
