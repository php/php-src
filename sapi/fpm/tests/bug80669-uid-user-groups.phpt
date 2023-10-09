--TEST--
FPM: Process user setting ignored when FPM is not running as root
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfNotRoot();
FPM\Tester::skipIfUserDoesNotExist('www-data');
?>
--FILE--
<?php

require_once "tester.inc";

$pw = posix_getpwnam('www-data');
$uid = $pw['uid'];
$gid = $pw['gid'];

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
user = $uid
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
echo posix_getgid();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody((string) $gid);
$tester->terminate();
$tester->expectLogTerminatingNotices();
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
