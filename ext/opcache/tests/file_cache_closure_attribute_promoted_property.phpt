--TEST--
opcache file cache: closure in attribute argument on promoted property (attribute compiled twice)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FIXME');
if (!function_exists('opcache_is_script_cached_in_file_cache')) die('skip opcache.file_cache not available');
$dir = __DIR__ . '/file_cache_closure_attribute_promoted_property';
if (!@mkdir("$dir/cache", 0777, true)) die('skip cannot create cache directory');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/file_cache_closure_attribute_promoted_property/cache"
opcache.file_update_protection=0
--FILE--
<?php
$dir = __DIR__ . '/file_cache_closure_attribute_promoted_property';
$cache = "$dir/cache";
$script = "$dir/attr.php";

// For a promoted property the attribute is compiled twice (parameter and property);
// both copies reference the same constant-expression AST that holds the closure's op_array.
file_put_contents($script, <<<'PHP'
<?php
class Callback
{
    public function __construct(
        #[\MyAttribute(static function (): string { return 'closure-from-cache'; })]
        public array $data = [],
    ) {}
}
PHP);
$mtime = time() - 10;
touch($script, $mtime);

var_dump(opcache_compile_file($script));
var_dump(opcache_is_script_cached_in_file_cache($script));

// Replace the source: from now on the script can only be served from the file cache.
file_put_contents($script, '<?php throw new Error("script was recompiled instead of being loaded from the file cache");');
touch($script, $mtime);

$check = <<<'PHP'
$param = (new ReflectionParameter([Callback::class, '__construct'], 0))->getAttributes()[0]->getArguments()[0];
$prop  = (new ReflectionProperty(Callback::class, 'data'))->getAttributes()[0]->getArguments()[0];
echo "param: ", $param(), "\n";
echo "prop: ",  $prop(),  "\n";
PHP;

// Same process: opcache_compile_file() has already declared the class, evaluate the attribute
// arguments of the in-memory copy that was just serialized.
echo "in-process:\n";
eval($check);

// Fresh process.
$cmd = [PHP_BINARY, '-n'];
$ext = ini_get('extension_dir');
if (file_exists($ext . DIRECTORY_SEPARATOR . 'opcache.so')) {
    array_push($cmd, '-d', 'zend_extension=opcache');
}
array_push($cmd,
    '-d', 'opcache.enable_cli=1',
    '-d', 'opcache.file_cache="' . $cache . '"',
    '-d', 'opcache.file_cache_only=1',
    '-d', 'opcache.file_update_protection=0',
    '-r', 'require $argv[1]; ' . $check,
    $script,
);
$proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
echo "child process:\n", stream_get_contents($pipes[1]);
fclose($pipes[1]);
$stderr = stream_get_contents($pipes[2]);
fclose($pipes[2]);
echo "child exit code: ", proc_close($proc), "\n";
if ($stderr !== '') { echo "stderr: $stderr"; }
?>
--CLEAN--
<?php
function removeDirRecursive(string $dir): void {
    if (!is_dir($dir)) return;
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST,
    );
    foreach ($iterator as $fileinfo) {
        $fileinfo->isDir() ? rmdir($fileinfo->getPathname()) : unlink($fileinfo->getPathname());
    }
    rmdir($dir);
}
removeDirRecursive(__DIR__ . '/file_cache_closure_attribute_promoted_property');
?>
--EXPECT--
bool(true)
bool(true)
in-process:
param: closure-from-cache
prop: closure-from-cache
child process:
param: closure-from-cache
prop: closure-from-cache
child exit code: 0
