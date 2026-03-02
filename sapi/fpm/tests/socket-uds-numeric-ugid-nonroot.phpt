--TEST--
FPM: UNIX socket owner and group settings can be numeric
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfRoot();
FPM\Tester::skipIfPosixNotLoaded();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
listen.owner = {{UID}}
listen.group = {{GID}}
user = {{USER}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig();
$tester->start();
$tester->expectLogNotice(
    "'user' directive is ignored when FPM is not running as root",
    'unconfined'
);
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
--EXPECTF--
%d/%s,%d/%s
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
