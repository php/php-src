--TEST--
FPM: Apparmor Test
--DESCRIPTION--
This test tries to launches a pool which tries to change to non existing
apparmor hat a. Test succeeds if apparmor is not running or hat is non
existent.
--SKIPIF--
<?php
include "skipif.inc";
include "skipapparmor.inc";

?>
--FILE--
<?php

include "include.inc";

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

/* libapparmor has a bug which can cause SIGSEGV till Version 2.8.0-0ubuntu28
   See https://bugs.launchpad.net/apparmor/+bug/1196880
   Possible outcomes:

   - SIGSEGV|failed to query apparmor confinement
     apparmor not running
   - failed to change to new confinement
     something in apparmor went wrong
   - exited with code 70
     Change to successful; Hat not existent (Process gets killed by apparmor)
 */
var_dump(run_fpm_till('/(SIGSEGV|failed to query apparmor confinement|failed to change to new confinement|exited with code 70)/', $cfg));

?>
--EXPECTF--
string(%d) "%s
"
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
?>
