--TEST--
FPM: AppArmor basic test
--SKIPIF--
<?php
include "skipif.inc";
$config = <<<EOT
[global]
error_log = /dev/null
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
apparmor_hat = a
EOT;
FPM\Tester::skipIfConfigFails($config);
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
apparmor_hat = a
EOT;

$tester = new FPM\Tester($cfg);
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
$tester->runTill(
    '/(SIGSEGV|failed to query apparmor confinement|' .
    'failed to change to new confinement|exited with code 70)/'
);

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
