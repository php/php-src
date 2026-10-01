--TEST--
socket_sendmsg(): SCM_RIGHTS transfers Socket objects with the correct fds
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (strtolower(substr(PHP_OS, 0, 3)) == 'win') {
    die('skip not for Microsoft Windows');
}
if (strtolower(substr(PHP_OS, 0, 3)) == 'aix') {
    die('skip not for AIX');
}
if (!defined('SCM_RIGHTS')) {
    die('skip SCM_RIGHTS not available');
}
?>
--FILE--
<?php
$dir = sys_get_temp_dir();
$rpath = $dir . "/socket_sendmsg_scm_rights_object_r.sock";
$ppath1 = $dir . "/socket_sendmsg_scm_rights_object_p1.sock";
$ppath2 = $dir . "/socket_sendmsg_scm_rights_object_p2.sock";
@unlink($rpath);
@unlink($ppath1);
@unlink($ppath2);

$recv = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_bind($recv, $rpath);

$send = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_connect($send, $rpath);

// Two Socket objects in a single SCM_RIGHTS message, each bound to a known
// path so the received fds can be identified and ordered via getsockname().
$payload1 = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_bind($payload1, $ppath1);
$payload2 = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_bind($payload2, $ppath2);

socket_sendmsg($send, [
    'iov' => ['x'],
    'control' => [[
        'level' => SOL_SOCKET,
        'type' => SCM_RIGHTS,
        'data' => [$payload1, $payload2],
    ]],
], 0);

$data = [
    'name' => [],
    'buffer_size' => 64,
    'controllen' => socket_cmsg_space(SOL_SOCKET, SCM_RIGHTS, 2),
];
socket_recvmsg($recv, $data, 0);

$got = $data['control'][0]['data'];
var_dump(count($got));
var_dump($got[0] instanceof Socket);
var_dump($got[1] instanceof Socket);
socket_getsockname($got[0], $addr0);
socket_getsockname($got[1], $addr1);
var_dump($addr0 === $ppath1);
var_dump($addr1 === $ppath2);
?>
--CLEAN--
<?php
$dir = sys_get_temp_dir();
@unlink($dir . "/socket_sendmsg_scm_rights_object_r.sock");
@unlink($dir . "/socket_sendmsg_scm_rights_object_p1.sock");
@unlink($dir . "/socket_sendmsg_scm_rights_object_p2.sock");
?>
--EXPECT--
int(2)
bool(true)
bool(true)
bool(true)
bool(true)
