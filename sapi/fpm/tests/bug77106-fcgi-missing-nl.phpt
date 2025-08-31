--TEST--
FPM: bug77106 - Missing new lines in FCGI error stream
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
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_admin_flag[log_errors] = 1
EOT;

$code = <<<EOT
<?php
echo \$a;
echo \$a;
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectErrorPattern(
    '/Undefined variable \$a in .+ on line \d+; PHP message: PHP Warning:/'
);
$tester->terminate();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
