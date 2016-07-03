<?php

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$cfg = <<<EOT
[global]
error_log = $logfile
[a]
listen = 127.0.0.1:9001
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
apparmor_hat = a
EOT;

$fpm = run_fpm($cfg, $out, '-t');
$ok = false;
if (is_resource($fpm)) {
    if (strpos(stream_get_contents($out), "test is successful") !== FALSE) {
        $ok = true;
    }
    fclose($out);
    proc_close($fpm);
}
if (!$ok) {
    die("skip No apparmor support built in");
}

?>
