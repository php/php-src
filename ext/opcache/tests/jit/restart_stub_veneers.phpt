--TEST--
JIT stub handlers must not point to veneers discarded by an OPcache restart
--DESCRIPTION--
On AArch64, traces that are out of branch range of a stub get a veneer, and the address of the
stub in zend_jit_stub_handlers[] is replaced by the address of that veneer. Code compiled after a
restart must not branch to veneers in the discarded part of the JIT buffer. Needs more than 1 MB
of JIT code, and different code after the restart than before it.
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (ini_get('opcache.jit') === false) die('skip PHP is compiled without JIT');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
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
$dir = __DIR__ . '/restart_stub_veneers';
@mkdir($dir);
@mkdir("$dir/a");
@mkdir("$dir/b");

/* Workload A: classes with hot loops */
for ($i = 0; $i < 200; $i++) {
    $src = "<?php\nclass C$i {\n";
    for ($m = 0; $m < 6; $m++) {
        $src .= "  public static function m$m(array \$a, int \$k): int {\n"
            . "    \$s = $i;\n"
            . "    foreach (\$a as \$j => \$v) { if ((\$v + \$k) % " . ($m + 2) . " === 0) { \$s += \$v * $m; } else { \$s ^= \$j; } \$s .= ''; \$s = (int)\$s; }\n"
            . "    \$str = str_repeat('x', \$k % 7); \$s += strlen(\$str);\n"
            . "    return \$s;\n  }\n";
    }
    file_put_contents("$dir/a/C$i.php", $src . "}\n");
}

/* Workload B: polymorphic branches, closures and caught exceptions */
for ($i = 0; $i < 400; $i++) {
    $src = "<?php\nclass P$i implements ArrayAccess {\n  private array \$d = [];\n"
        . "  public function offsetExists(mixed \$o): bool { return isset(\$this->d[\$o]); }\n"
        . "  public function offsetGet(mixed \$o): mixed { return \$this->d[\$o] ?? null; }\n"
        . "  public function offsetSet(mixed \$o, mixed \$v): void { \$this->d[\$o] = \$v; }\n"
        . "  public function offsetUnset(mixed \$o): void { unset(\$this->d[\$o]); }\n"
        . "  public function run(array \$a): mixed { \$r = 0; foreach (\$a as \$k => \$v) {\n"
        . "    if (is_int(\$v)) { \$r += \$v * $i; } elseif (is_string(\$v)) { \$r .= substr(\$v, 0, 2); } elseif (is_array(\$v)) { \$r = count(\$v) + (int)\$r; } else { \$r = \$v ?? \$r; }\n"
        . "    \$this[\$k % 17] = \$r; \$x = fn(\$y) => \$y . \$k; \$r = is_string(\$r) ? strlen(\$x(\$r)) : \$r; }\n"
        . "    try { if (\$r % " . (($i % 9) + 2) . " === 0) { throw new RuntimeException('x'); } } catch (Throwable \$e) { \$r++; }\n"
        . "    return \$r; }\n}\n";
    file_put_contents("$dir/b/P$i.php", $src);
}

file_put_contents("$dir/a.inc", <<<'PHP'
<?php
$a = range(1, 200);
for ($i = 0; $i < 200; $i++) {
    require_once __DIR__ . "/a/C$i.php";
    $c = "C$i";
    for ($r = 0; $r < 4; $r++) {
        for ($m = 0; $m < 6; $m++) { $c::{"m$m"}($a, $r + $m); }
    }
}
PHP);
file_put_contents("$dir/b.inc", <<<'PHP'
<?php
$mix = [];
for ($j = 0; $j < 120; $j++) { $mix[] = [$j, "s$j", [$j], null, 1.5][$j % 5]; }
for ($i = 0; $i < 400; $i++) {
    require_once __DIR__ . "/b/P$i.php";
    $c = "P$i";
    (new $c())->run($mix);
}
PHP);
file_put_contents("$dir/b.php", "<?php\nrequire __DIR__ . '/b.inc';\necho \"b ok\\n\";\n");
file_put_contents("$dir/ab.php", "<?php\nrequire __DIR__ . '/a.inc';\nrequire __DIR__ . '/b.inc';\necho \"ab ok\\n\";\n");
file_put_contents("$dir/reset.php", "<?php\nvar_dump(opcache_reset());\n");

include __DIR__ . '/../php_cli_server.inc';

$ini = '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.jit=tracing -d opcache.jit_buffer_size=64M'
    . ' -d opcache.jit_max_root_traces=100000 -d opcache.jit_max_side_traces=100000'
    . ' -d opcache.jit_max_exit_counters=65536 -d opcache.validate_timestamps=0'
    . ' -d opcache.file_update_protection=0 -d error_reporting=' . (E_ALL & ~E_DEPRECATED);
$ext = ini_get('extension_dir');
if (file_exists($ext . DIRECTORY_SEPARATOR . (PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so'))) {
    $ini .= ' -d extension_dir=' . $ext . ' -d zend_extension=opcache';
}
php_cli_server_start($ini);

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/jit/restart_stub_veneers/';
foreach (['b', 'b', 'reset', 'ab', 'ab'] as $script) {
    echo @file_get_contents($base . $script . '.php') ?: "no response\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/restart_stub_veneers';
foreach (array_merge(glob("$dir/a/*.php"), glob("$dir/b/*.php")) as $file) {
    @unlink($file);
}
@rmdir("$dir/a");
@rmdir("$dir/b");
foreach (['a.inc', 'b.inc', 'b.php', 'ab.php', 'reset.php'] as $file) {
    @unlink("$dir/$file");
}
@rmdir($dir);
?>
--EXPECT--
b ok
b ok
bool(true)
ab ok
ab ok
