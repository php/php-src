--TEST--
GH-13989: file_cache_only Docker deployment — ship image with only opcache binary cache, no source
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip Docker test requires Linux host');
}
exec('docker info 2>&1', $output, $exitCode);
if ($exitCode !== 0) {
    die('skip Docker is not available or not running');
}
$opcacheSo = ini_get('extension_dir') . '/opcache.so';
if (!file_exists($opcacheSo)) {
    die('skip opcache.so not found');
}
?>
--INI--
opcache.enable=0
--FILE--
<?php
$phpBin = PHP_BINARY;
$opcacheSo = ini_get('extension_dir') . '/opcache.so';

// Collect shared library directories from ldd so the host PHP binary
// can run inside the container without installing build dependencies.
$libDirs = [];
foreach ([$phpBin, $opcacheSo] as $binary) {
    $lines = [];
    exec('ldd ' . escapeshellarg($binary) . ' 2>/dev/null', $lines);
    foreach ($lines as $line) {
        if (preg_match('#=>\s+(/\S+)#', $line, $m) && file_exists($m[1])) {
            $libDirs[dirname($m[1])] = true;
        }
    }
}

// Mount each host library directory at /opt/hostlibs/N in the container
$mounts = '';
$ldPaths = [];
$idx = 0;
foreach (array_keys($libDirs) as $dir) {
    $target = '/opt/hostlibs/' . $idx;
    $mounts .= sprintf(' -v %s:%s:ro', escapeshellarg($dir), $target);
    $ldPaths[] = $target;
    $idx++;
}
$ldPathStr = implode(':', $ldPaths);

$bashScript = <<<'BASH'
#!/bin/bash
set -e
export LD_LIBRARY_PATH="%%LD_LIBRARY_PATH%%"

PHP=/opt/php
OPTS="-n -dzend_extension=/opt/opcache.so -dopcache.enable=1 -dopcache.enable_cli=1 -dopcache.jit=disable"
OPTS="$OPTS -dopcache.file_cache=/app/cache -dopcache.file_cache_only=1"
OPTS="$OPTS -dopcache.file_update_protection=0 -dopcache.validate_timestamps=0"

# ── Create a Composer-like app with classmap autoloader ────────
mkdir -p /app/{cache,src/Models,src/Services,vendor/composer}

cat > /app/src/Models/User.php << 'PHP_SRC'
<?php
namespace App\Models;
class User {
    public function __construct(private string $name) {}
    public function getName(): string { return $this->name; }
}
PHP_SRC

cat > /app/src/Services/Greeter.php << 'PHP_SRC'
<?php
namespace App\Services;
use App\Models\User;
class Greeter {
    public function greet(User $u): string {
        return "Hello, " . $u->getName() . "!";
    }
}
PHP_SRC

cat > /app/vendor/composer/autoload_classmap.php << 'PHP_SRC'
<?php return array(
    'App\\Models\\User' => '/app/src/Models/User.php',
    'App\\Services\\Greeter' => '/app/src/Services/Greeter.php',
);
PHP_SRC

cat > /app/vendor/autoload.php << 'PHP_SRC'
<?php
$classMap = require __DIR__ . '/composer/autoload_classmap.php';
spl_autoload_register(function ($class) use ($classMap) {
    if (isset($classMap[$class])) {
        require $classMap[$class];
    }
});
PHP_SRC

cat > /app/index.php << 'PHP_SRC'
<?php
require __DIR__ . '/vendor/autoload.php';
$user = new \App\Models\User('Docker');
$greeter = new \App\Services\Greeter();
echo $greeter->greet($user) . "\n";
PHP_SRC

# ── Step 1: Warm the file cache ───────────────────────────────
$PHP $OPTS /app/index.php

# ── Step 2: Remove class source files (simulate stripped deploy)
rm /app/src/Models/User.php /app/src/Services/Greeter.php

# ── Step 3: Run from cache — class files served by opcache ────
$PHP $OPTS /app/index.php

# ── Step 4: Remove ALL source files ───────────────────────────
rm /app/index.php /app/vendor/autoload.php /app/vendor/composer/autoload_classmap.php

# ── Step 5: Run from cache — everything served by opcache ─────
$PHP $OPTS /app/index.php
BASH;

$bashScript = str_replace('%%LD_LIBRARY_PATH%%', $ldPathStr, $bashScript);

$scriptFile = tempnam(sys_get_temp_dir(), 'gh13989_docker_');
file_put_contents($scriptFile, $bashScript);

$cmd = sprintf(
    'docker run --rm%s -v %s:/opt/php:ro -v %s:/opt/opcache.so:ro -v %s:/test.sh:ro debian:bookworm-slim bash /test.sh 2>/dev/null',
    $mounts,
    escapeshellarg($phpBin),
    escapeshellarg($opcacheSo),
    escapeshellarg($scriptFile)
);

$out = shell_exec($cmd);
@unlink($scriptFile);

echo $out;
?>
--EXPECT--
Hello, Docker!
Hello, Docker!
Hello, Docker!
