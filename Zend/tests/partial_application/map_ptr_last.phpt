--TEST--
PFA and ZCSG(map_ptr_last)
--CREDITS--
Ryan @ Calif.io
--EXTENSIONS--
pcntl
--INI--
opcache.file_update_protection=0
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
    die("skip no pcntl_fork");
}
?>
--FILE--
<?php

function f($a, $b) {
    return new ReflectionClass($a);
}

function get_pfa() {
    return f(?, 0);
}

$pid = pcntl_fork();
if ($pid) {
    pcntl_waitpid($pid, $status, 0);

    /* PFA retrieved from SHM, run_time_cache > CG(map_ptr_last) */
    var_dump(get_pfa()('ReflectionClass'));
} else {
    /* Increment ZSCG(map_ptr_last) enough to mandate a realloc in other processes.
     * We do so by compiling many functions that are guaranteed to allocate a map_ptr. */

    $fd = fopen(__DIR__ . '/map_ptr_last.inc', 'w');
    fwrite($fd, "<?php\n");
    for ($i = 0; $i < 4096; $i++) {
        fprintf($fd, "function pfa_map_ptr_flood_%05d(\$obj) { return \$obj->prop; }\n", $i);
    }
    fclose($fd);

    require __DIR__ . '/map_ptr_last.inc';

    /* Compile a PFA */
    get_pfa()('ReflectionClass');
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/map_ptr_last.inc');
?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(15) "ReflectionClass"
}
