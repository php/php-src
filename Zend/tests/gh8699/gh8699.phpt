--TEST--
GH-8699:Wrong value from ini_get() for shared files because of opcache optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache={PWD}/cache
opcache.file_cache_only=1
opcache.jit=0
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$extraArgs = getenv('TEST_PHP_EXTRA_ARGS');
$pwd = __DIR__;

if (strpos($extraArgs, 'opcache.so') === false) {
    $opcacheFile = dirname($php, 3) . '/modules/opcache.so';
    $extraArgs = "-dzend_extension=$opcacheFile $extraArgs";
}

$cmd1 = "$php $extraArgs -d \"upload_tmp_dir=/tmp/num1\" " . __DIR__ . '/1.php';
$cmd2 = "$php $extraArgs -d \"upload_tmp_dir=/tmp/num2\" " . __DIR__ . '/2.php';

passthru($cmd1);
passthru($cmd2);

// Make sure we actually used the opcache
$pattern = __DIR__ . '/cache/*' . str_replace(':', '', __DIR__) . '/shared.php.bin';
$cachedFiles = glob($pattern);
if (count($cachedFiles) !== 1) {
    echo "Opcache did not cache the shared php file\n";
}
--CLEAN--
<?php
$pattern = __DIR__ . '/cache/*' . str_replace(':', '', __DIR__) . '/*.php.bin';
foreach (glob($pattern) as $p) {
    unlink($p);
}

$dir = glob(__DIR__ . '/cache/*' . str_replace(':', '', __DIR__))[0] ?? null;
if ($dir ) {
    $baseLen = strlen(__DIR__ . '/cache');
    while(strlen($dir) > $baseLen) {
        rmdir($dir);
        $dir = dirname($dir);
    }
}
--EXPECT--
1: /tmp/num1
shared: /tmp/num1
2: /tmp/num2
shared: /tmp/num2
