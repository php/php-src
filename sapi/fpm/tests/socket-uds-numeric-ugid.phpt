--TEST--
FPM: UNIX socket owner and group settings can be numeric
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfPosixNotLoaded();
FPM\Tester::skipIfNotRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
listen.owner = 1234
listen.group = 1234
user = 1234
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->ping('{{ADDR:UDS}}');
$st = stat($tester->getListen('{{ADDR:UDS}}'));
if ($st) {
  $pw = posix_getpwuid($st['uid']);
  $gr = posix_getgrgid($st['gid']);
  $user  = $pw ? $pw['name'] : 'UNKNOWN';
  $group = $gr ? $gr['name'] : 'UNKNOWN';
  echo "{$st['uid']}/{$user},{$st['gid']}/{$group}\n";
} else {
  echo "stat failed for " . $tester->getListen('{{ADDR:UDS}}');
}
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
1234/UNKNOWN,1234/UNKNOWN
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
