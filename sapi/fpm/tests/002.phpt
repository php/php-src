--TEST--
FPM: Startup and connect
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$port = 9000+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
[unconfined]
listen = 127.0.0.1:$port
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, 2);
    $i = 0;
    while (($i++ < 60) && !($fp = @fsockopen('127.0.0.1', $port))) {
        usleep(50000);
    }
    if ($fp) {
        echo "Done\n";
        fclose($fp);
    }
    proc_terminate($fpm);
    stream_get_contents($tail);
    fclose($tail);
    proc_close($fpm);
}

?>
--EXPECTF--
[%d-%s-%d %d:%d:%d] NOTICE: fpm is running, pid %d
[%d-%s-%d %d:%d:%d] NOTICE: ready to handle connections
Done
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
?>
