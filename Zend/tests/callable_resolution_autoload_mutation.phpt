--TEST--
Autoloading must not invalidate callables during callable resolution
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    if ($class === 'IsCallableAutoloadTest') {
        $GLOBALS['method1'] = 'missing';
        eval('class IsCallableAutoloadTest {
            public static function original(): void {}
        }');
    } elseif ($class === 'CallUserFuncAutoloadTest') {
        $GLOBALS['method2'] = 'changed';
        eval('class CallUserFuncAutoloadTest {
            public static function original(): void { echo "original\\n"; }
            public static function changed(): void { echo "changed\\n"; }
        }');
    } elseif ($class === 'CallUserFuncStringAutoloadTest') {
        $GLOBALS['callable3'][strlen($GLOBALS['callable3']) - 1] = 'o';
        eval('class CallUserFuncStringAutoloadTest {
            public static function mOne(): void { echo "mOne\\n"; }
            public static function mOno(): void { echo "mOno\\n"; }
        }');
    }
});

$method1 = 'original';
$callable1 = ['IsCallableAutoloadTest', &$method1];
var_dump(is_callable($callable1));

$method2 = 'original';
$callable2 = ['CallUserFuncAutoloadTest', &$method2];
call_user_func($callable2);

$callable3 = strrev('enOm::tseTdaolotuAgnirtScnuFresUllaC');
$alias3 = &$callable3;
call_user_func($alias3);
?>
--EXPECT--
bool(true)
original
mOne
