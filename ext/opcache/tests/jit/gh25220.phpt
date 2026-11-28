--TEST--
GH-25220 (JIT hot counters segfault when shared memory is exhausted during script JIT setup)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip Windows SHM accounting differs');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
opcache.memory_consumption=8
opcache.max_accelerated_files=10000
opcache.interned_strings_buffer=1
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.jit_max_root_traces=1024
opcache.jit_max_side_traces=128
opcache.jit_max_exit_counters=8192
memory_limit=-1
--FILE--
<?php
$dir = sys_get_temp_dir() . '/gh25220';
@mkdir($dir, 0777, true);

$n = 150;
$methods = 50;

for ($i = 0; $i < $n; $i++) {
    $calls = '';
    $m = '';
    for ($k = 1; $k < $methods; $k++) {
        $calls .= "        \$a += \$this->m$k(\$x + $k);\n";
        $m .= "    public function m$k(int \$x): int { \$t = \$x + $k; return \$t > 3 ? \$t : \$t + $k; }\n";
    }
    file_put_contents("$dir/C$i.php",
        "<?php\nfinal class C$i {\n    public function m0(int \$x): int {\n        \$a = 0;\n$calls        return \$a;\n    }\n$m}\n"
    );
}

$sum = 0;
for ($pass = 0; $pass < 2; $pass++) {
    for ($i = 0; $i < $n; $i++) {
        if ($pass === 0) {
            require "$dir/C$i.php";
        }
        $class = "C$i";
        $o = new $class();
        $sum += $o->m0(1);
        $sum += $o->m0(2);
    }
}

echo "ok\n";
?>
--CLEAN--
<?php
$dir = sys_get_temp_dir() . '/gh25220';
foreach (glob($dir . '/*.php') ?: [] as $f) {
    @unlink($f);
}
@rmdir($dir);
?>
--EXPECT--
ok
