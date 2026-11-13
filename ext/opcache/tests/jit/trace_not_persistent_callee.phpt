--TEST--
Tracing JIT must not specialize calls to functions that are not persistent
--DESCRIPTION--
X::run() comes from a blacklisted (never cached) file, so its op_array and opcodes are allocated
per request. Both versions of the file have the same allocation pattern, so after the first
request run() gets the same opcodes address in every request. A trace of work() recorded with
version A must not be used for version B (more CVs, bigger call frame).
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (ini_get('opcache.jit') === false) die('skip PHP is compiled without JIT');
if (getenv('USE_ZEND_ALLOC') === '0') die('skip Zend MM disabled');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=16M
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/trace_not_persistent_callee';
@mkdir($dir);

$names = [];
for ($k = 0; $k < 26; $k++) {
    $names[] = "'v$k'";
}
/* names() is never called: it makes the CV names of x.php permanent interned strings, so both
 * versions of x.php allocate the same request memory */
file_put_contents($dir . '/work.php', '<?php
function work(object $o, int $n): int {
    $s = 0;
    for ($i = 0; $i < $n; $i++) {
        $s += $o->run($i);
    }
    return $s;
}
function names(): array {
    return compact(' . implode(', ', $names) . ');
}
');

/* Same number of opcodes; version A uses 17 CVs, version B 26 CVs */
function x_version(int $cvs): string {
    $code = "<?php\nclass X {\n    public function run(int \$i): int {\n";
    $sum = [];
    for ($k = 0; $k < 26; $k++) {
        $code .= "        \$v" . ($k % $cvs) . " = \$i + $k;\n";
        $sum[] = "\$v" . ($k % $cvs);
    }
    return $code . "        return " . implode(' + ', $sum) . " + intdiv(\$i, 3);\n    }\n}\n";
}
file_put_contents($dir . '/a.php', x_version(17));
file_put_contents($dir . '/b.php', x_version(26));
file_put_contents($dir . '/index.php', '<?php
require __DIR__ . "/work.php";
require __DIR__ . "/x.php";
echo work(new X(), (int) $_GET["n"]), "\n";
');
file_put_contents($dir . '/blacklist.txt', $dir . "/x.php\n");

include __DIR__ . '/../php_cli_server.inc';

$ini = '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.jit=tracing -d opcache.jit_buffer_size=16M'
    . ' -d opcache.file_update_protection=0 -d opcache.blacklist_filename=' . $dir . '/blacklist.txt';
if (file_exists(ini_get('extension_dir') . '/opcache.' . PHP_SHLIB_SUFFIX)) {
    $ini .= ' -d zend_extension=opcache.' . PHP_SHLIB_SUFFIX;
}
php_cli_server_start($ini);

$url = 'http://' . PHP_CLI_SERVER_ADDRESS . '/jit/trace_not_persistent_callee/index.php?n=';
foreach ([['a', 1], ['a', 1000], ['b', 1000], ['a', 1000], ['b', 1000]] as [$version, $n]) {
    copy("$dir/$version.php", "$dir/x.php");
    echo $version, ' ', @file_get_contents($url . $n) ?: "no response\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/trace_not_persistent_callee';
foreach (['work.php', 'a.php', 'b.php', 'x.php', 'index.php', 'blacklist.txt'] as $file) {
    @unlink("$dir/$file");
}
@rmdir($dir);
?>
--EXPECT--
a 478
a 13631167
b 13478167
a 13631167
b 13478167
