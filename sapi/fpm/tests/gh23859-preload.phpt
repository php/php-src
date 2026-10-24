--TEST--
FPM: GH-23859 (Preloaded map pointers survive compilation and OPcache restarts)
--EXTENSIONS--
opcache
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[worker]
listen = {{ADDR[worker]}}
pm = static
pm.max_children = 1
[reset]
listen = {{ADDR[reset]}}
pm = static
pm.max_children = 1
EOT;

$code = <<<'EOT'
<?php
if (PreloadedChild::$value !== 7 || PreloadedChild::$items !== ['initial']
    || PreloadedChild::step() !== 8 || preloaded_step() !== 13
    || PreloadedEnum::cases() !== [PreloadedEnum::A]
    || (new ReflectionClass(PreloadedMutable::class))->getDefaultProperties() !== ['item' => 11]) {
    throw new Exception('Preloaded state was not reset');
}
PreloadedChild::$value = 23;
PreloadedChild::$items[] = 'changed';

/* Persist new code and load a saved preload script after filling preload slots. */
require __DIR__ . '/gh23859-preload.compile.inc';
require __DIR__ . '/gh23859-preload.saved.inc';
if (PreloadedChild::$value !== 23 || PreloadedChild::$items !== ['initial', 'changed']
    || PreloadedChild::step() !== 25 || preloaded_step() !== 15
    || PreloadedChild::compute() !== 13 || RequestChild::run() !== 20
    || request_step() !== 1 || saved_step() !== 1
    || PreloadedEnum::cases() !== [PreloadedEnum::A]) {
    throw new Exception('Compilation corrupted preloaded state');
}
echo 'ok';
EOT;

$tester = new FPM\Tester($cfg, $code);
$preloadFile = $tester->getPrefixedFile('preload.inc');
file_put_contents($preloadFile, <<<'EOT'
<?php
trait PreloadedTrait {
    public static function step(): int {
        static $n = 0;
        return ++$n + self::$value;
    }
}
class PreloadedBase {
    public static int $value = 7;
    public const VALUE = 11;
}
class PreloadedChild extends PreloadedBase {
    use PreloadedTrait;
    public static array $items = ['initial'];
    public static function compute(): int {
        return self::VALUE + count(self::$items);
    }
}
function preloaded_step(): int {
    static $n = 0;
    return ++$n + PreloadedChild::compute();
}
enum PreloadedEnum { case A; }
trait PreloadedMutable { public $item = PreloadedBase::VALUE; }
opcache_compile_file(__DIR__ . '/gh23859-preload.saved.inc');
PreloadedChild::step();
preloaded_step();
PreloadedChild::$value = 99;
EOT);
file_put_contents($tester->getPrefixedFile('compile.inc'), <<<'EOT'
<?php
class RequestChild extends PreloadedChild {
    public static int $other = 19;
    public static function run(): int { static $n = 0; return ++$n + self::$other; }
}
function request_step(): int { static $n = 0; return ++$n; }
EOT);
file_put_contents($tester->getPrefixedFile('saved.inc'), <<<'EOT'
<?php
static $n = 0;
if (++$n !== 1) {
    throw new Exception('Saved script static variables were not reset');
}
if (!function_exists('saved_step')) {
    function saved_step(): int { static $n = 0; return ++$n; }
}
EOT);
$resetFile = $tester->getPrefixedFile('reset.src.php');
file_put_contents($resetFile, <<<'EOT'
<?php
if (isset($_GET['reset'])) {
    var_dump(opcache_reset());
} else {
    echo opcache_get_status(false)['opcache_statistics']['manual_restarts'];
}
EOT);

$opcache = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'opcache.' . PHP_SHLIB_SUFFIX;
$extraArgs = is_file($opcache) ? ['-dzend_extension=' . $opcache] : [];
$iniEntries = [
    'opcache.enable' => '1',
    'opcache.file_update_protection' => '0',
    'opcache.preload' => $preloadFile,
];
if (getenv('TEST_FPM_RUN_AS_ROOT')) {
    $iniEntries['opcache.preload_user'] = 'root';
}
$tester->start($extraArgs, iniEntries: $iniEntries);
$tester->expectLogStartNotices();
$tester->request(address: '{{ADDR[reset]}}', scriptFilename: $resetFile)->expectBody('0');
$tester->request(address: '{{ADDR[worker]}}')->expectBody('ok');
$tester->request(address: '{{ADDR[worker]}}')->expectBody('ok');

for ($i = 1; $i <= 3; $i++) {
    $tester->request(query: 'reset=1', address: '{{ADDR[reset]}}', scriptFilename: $resetFile)
        ->expectBody('bool(true)');
    $tester->request(address: '{{ADDR[reset]}}', scriptFilename: $resetFile)->expectBody((string) $i);
    $tester->request(address: '{{ADDR[worker]}}')->expectBody('ok');
    $tester->request(address: '{{ADDR[worker]}}')->expectBody('ok');
}

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
foreach (['preload', 'compile', 'saved'] as $file) {
    @unlink(__DIR__ . '/gh23859-preload.' . $file . '.inc');
}
@unlink(__DIR__ . '/gh23859-preload.reset.src.php');
?>
