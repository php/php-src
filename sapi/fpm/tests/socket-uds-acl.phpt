--TEST--
FPM: Unix Domain Socket with Posix ACL
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfAnyFileDoesNotExist(['/usr/bin/getfacl', '/etc/passwd', '/etc/group']);
$config = <<<EOT
[global]
error_log = /dev/null
[unconfined]
listen = {{ADDR}}
listen.acl_users = nobody
listen.acl_groups = nobody
listen.mode = 0600
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;
FPM\Tester::skipIfConfigFails($config);
?>
--FILE--
<?php

require_once "tester.inc";

// Select 3 users and 2 groups known by system (avoid root)
$users = $groups = [];
$tmp = file('/etc/passwd');
for ($i=1 ; $i <= 3 ; $i++) {
    $tab = explode(':', $tmp[$i]);
    $users[] = $tab[0];
}
$users = implode(',', $users);
$tmp = file('/etc/group');
for ($i=1 ; $i <= 2 ; $i++) {
    $tab = explode(':', $tmp[$i]);
    $groups[] = $tab[0];
}
$groups = implode(',', $groups);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
listen.acl_users = $users
listen.acl_groups = $groups
listen.mode = 0600
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
passthru("/usr/bin/getfacl -cp " . $tester->getListen('{{ADDR:UDS}}'));
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECTF--
user::rw-
user:%s:rw-
user:%s:rw-
user:%s:rw-
group::---
group:%s:rw-
group:%s:rw-
mask::rw-
other::---

Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
