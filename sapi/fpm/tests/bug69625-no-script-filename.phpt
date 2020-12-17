--TEST--
FPM: bug69625 - 404 should be returned on missing SCRIPT_FILENAME
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
EOT;

$code = <<<EOT
<?php
echo "Test\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request('', ['SCRIPT_FILENAME' => null])
    ->expectHeader('Status', '404 Not Found')
    ->expectError('Primary script unknown');
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
