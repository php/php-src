--TEST--
FPM: Test Unix Domain Socket with Posix ACL
--SKIPIF--
<?php
include "skipif.inc";
if (!(file_exists('/usr/bin/getfacl') && file_exists('/etc/passwd') && file_exists('/etc/group'))) die ("skip missing getfacl command");
$cfg = <<<EOT
[global]
error_log = /dev/null
[unconfined]
listen = 127.0.0.1:9999
listen.acl_users = nobody
listen.acl_groups = nobody
listen.mode = 0600
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;
if (test_fpm_conf($cfg, $msg) == false) { die("skip " .  $msg); }
?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$socket  = dirname(__FILE__).'/php-fpm.sock';

// Select 3 users and 2 groups known by system (avoid root)
$users = $groups = [];
$tmp = file('/etc/passwd');
for ($i=1 ; $i<=3 ; $i++) {
	$tab = explode(':', $tmp[$i]);
	$users[] = $tab[0];
}
$users = implode(',', $users);
$tmp = file('/etc/group');
for ($i=1 ; $i<=2 ; $i++) {
	$tab = explode(':', $tmp[$i]);
	$groups[] = $tab[0];
}
$groups = implode(',', $groups);

$cfg = <<<EOT
[global]
error_log = $logfile
[unconfined]
listen = $socket
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

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, 2);
    try {
		var_dump(strpos(run_request('unix://'.$socket, -1), 'pong'));
		echo "UDS ok\n";
	} catch (Exception $e) {
		echo "UDS error\n";
	}
	passthru("/usr/bin/getfacl -cp $socket");

	proc_terminate($fpm);
    fpm_display_log($tail, -1);
    fclose($tail);
    proc_close($fpm);
}

?>
--EXPECTF--
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
int(%d)
UDS ok
user::rw-
user:%s:rw-
user:%s:rw-
user:%s:rw-
group::---
group:%s:rw-
group:%s:rw-
mask::rw-
other::---

[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
?>
