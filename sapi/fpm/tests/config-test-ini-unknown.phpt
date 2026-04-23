--TEST--
FPM: config test rejects unknown INI settings
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 5
php_admin_value[does_not_exist] = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig();

?>
Done
--EXPECT--
ERROR: [pool unconfined] Unknown 'does_not_exist' setting
ERROR: FPM initialization failed
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
