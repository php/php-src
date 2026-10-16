--TEST--
GH-23637: CE cache hit must extend map_ptr like other SHM paths
--EXTENSIONS--
opcache
pcntl
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_output=0
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
    die('skip no pcntl_fork');
}
if (PHP_OS_FAMILY === 'Windows') {
    die('skip fork/SHM race is not applicable on Windows');
}
?>
--FILE--
<?php

/*
 * Child fills SHM (bumping ZCSG(map_ptr_last)) and publishes an immutable
 * class into the CE cache. Parent must resolve that class through the CE
 * cache fast path without going through zend_accel_load_script (which would
 * already extend). With an fcall observer, missing the extend SEGV'd.
 */

$target = __DIR__ . '/gh23637_ce_cache_map_ptr.inc';
$flood = __DIR__ . '/gh23637_ce_cache_map_ptr_flood.inc';

$fd = fopen($target, 'w');
fwrite($fd, "<?php\nclass Gh23637CeCacheTarget {\n");
fwrite($fd, "    public function m() { return 'ok'; }\n");
fwrite($fd, "}\n");
fclose($fd);
touch($target, time() - 3600);

$pid = pcntl_fork();
if ($pid === 0) {
    $fd = fopen($flood, 'w');
    fwrite($fd, "<?php\n");
    for ($i = 0; $i < 4096; $i++) {
        fprintf($fd, "function gh23637_flood_%05d(\$o) { return \$o->p; }\n", $i);
    }
    fclose($fd);
    touch($flood, time() - 3600);

    require $flood;
    require $target;
    /* Arm CE cache and allocate method RTC map_ptr slots in SHM. */
    echo (new Gh23637CeCacheTarget)->m();
    exit(0);
}
if ($pid < 0) {
    echo "pcntl_fork() failed\n";
    exit(1);
}

pcntl_waitpid($pid, $status, 0);

/*
 * Do not require $target here: that would take zend_accel_load_script and
 * extend map_ptr. Resolve the class by name so zend_lookup_class_ex can hit
 * the CE cache published by the child.
 */
$name = 'Gh23637CeCacheTarget';
$obj = new $name;
echo $obj->m();
echo "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23637_ce_cache_map_ptr.inc');
@unlink(__DIR__ . '/gh23637_ce_cache_map_ptr_flood.inc');
?>
--EXPECT--
okok
