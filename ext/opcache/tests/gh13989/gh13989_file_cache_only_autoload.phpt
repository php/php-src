--TEST--
GH-13989: file_cache_only should serve autoloaded classes without source files (classmap autoloading)
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh13989_cache6', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache6"
opcache.file_cache_only=1
opcache.file_cache_consistency_checks=0
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.validate_permission=0
opcache.validate_root=0
--EXTENSIONS--
opcache
--FILE--
<?php
$php = PHP_BINARY;
$baseDir = __DIR__ . '/gh13989_autoload';
$cacheDir = __DIR__ . '/gh13989_cache6';

// Create project structure
@mkdir($baseDir . '/vendor/composer', 0777, true);
@mkdir($baseDir . '/src/Models', 0777, true);
@mkdir($baseDir . '/src/Services', 0777, true);

// Class files
file_put_contents($baseDir . '/src/Models/User.php', <<<'PHP'
<?php
namespace App\Models;
class User {
    public function __construct(private string $name) {}
    public function getName(): string { return $this->name; }
}
PHP);

file_put_contents($baseDir . '/src/Services/Greeter.php', <<<'PHP'
<?php
namespace App\Services;
use App\Models\User;
class Greeter {
    public function greet(User $user): string {
        return "Hello, " . $user->getName() . "!";
    }
}
PHP);

// Classmap autoloader (like composer dump-autoload -o)
file_put_contents($baseDir . '/vendor/composer/autoload_classmap.php',
    '<?php return array(' .
    "'App\\\\Models\\\\User' => dirname(__DIR__, 2) . '/src/Models/User.php'," .
    "'App\\\\Services\\\\Greeter' => dirname(__DIR__, 2) . '/src/Services/Greeter.php'," .
    ');'
);

file_put_contents($baseDir . '/vendor/autoload.php', <<<'PHP'
<?php
$classMap = require __DIR__ . '/composer/autoload_classmap.php';
spl_autoload_register(function ($class) use ($classMap) {
    if (isset($classMap[$class])) {
        require $classMap[$class];
    }
});
PHP);

// Main script
file_put_contents($baseDir . '/index.php', <<<'PHP'
<?php
require __DIR__ . '/vendor/autoload.php';
$user = new \App\Models\User('World');
$greeter = new \App\Services\Greeter();
echo $greeter->greet($user) . "\n";
PHP);

$opcacheArgs = implode(' ', [
    '-dopcache.enable=1',
    '-dopcache.enable_cli=1',
    '-dopcache.jit=disable',
    '-dopcache.file_cache=' . escapeshellarg($cacheDir),
    '-dopcache.file_cache_only=1',
    '-dopcache.file_cache_consistency_checks=0',
    '-dopcache.file_update_protection=0',
    '-dopcache.validate_timestamps=0',
    '-dopcache.validate_permission=0',
    '-dopcache.validate_root=0',
]);

$script = $baseDir . '/index.php';

// Step 1: Run to warm cache
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 1: " . trim($out) . "\n";

// Step 2: Delete class source files (keep autoloader and main script)
unlink($baseDir . '/src/Models/User.php');
unlink($baseDir . '/src/Services/Greeter.php');

// Step 3: Run again — autoloader will require files served from cache
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 2: " . trim($out) . "\n";

// Step 4: Also delete the main script and autoloader files
unlink($baseDir . '/index.php');
unlink($baseDir . '/vendor/autoload.php');
unlink($baseDir . '/vendor/composer/autoload_classmap.php');

// Step 5: Run with all source deleted
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 3: " . trim($out) . "\n";

echo "done\n";
?>
--CLEAN--
<?php
function removeDirRecursive($dir) {
    if (!is_dir($dir)) return;
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($iterator as $fileinfo) {
        if ($fileinfo->isDir()) {
            @rmdir($fileinfo->getRealPath());
        } else {
            @unlink($fileinfo->getRealPath());
        }
    }
    @rmdir($dir);
}
removeDirRecursive(__DIR__ . '/gh13989_cache6');
removeDirRecursive(__DIR__ . '/gh13989_autoload');
?>
--EXPECT--
Step 1: Hello, World!
Step 2: Hello, World!
Step 3: Hello, World!
done
