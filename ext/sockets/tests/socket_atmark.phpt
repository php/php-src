--TEST--
ext/sockets - socket_atmark
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (PHP_OS_FAMILY !== 'Windows') die("skip not for Windows");
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$path = sys_get_temp_dir() . "/socket_atmark.sock";

@unlink($path);

$string = "no chance of socket_atmark returning true";
$len = strlen($string);
$s_s = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("socket_create failed");
socket_set_nonblock($s_s) or die("socket_set_nonblock");

$s_c = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("socket_create failed");
socket_bind($s_c, $path) or die("socket_bind failed");
socket_set_nonblock($s_c) or die("socket_set_nonblock failed");
socket_set_option($s_c, SOL_SOCKET, SO_PASSCRED, 1) or die("could not set SO_PASSCRED");
$s_w = socket_sendto($s_s, $string, $len, 0, $path);
if ($s_w != $len)
    die("socket_send failed");
checktimeout($s_c, 500);
$data = [
    "name" => [],
    "buffer_size" => 2000,
    "controllen" => socket_cmsg_space(SOL_SOCKET, SCM_CREDENTIALS)
];

if (socket_atmark($s_c) === FALSE) {
    if (!socket_recvmsg($s_c, $data, 0))
        die("socket_recvmsg failed");
    echo "not OOB";
} else {
    $buf = "";
    if (!socket_recv($s_c, $buf, 1, MSG_OOB) === FALSE)
        die("socket_recv failed");
    echo "OOB";
}
socket_close($s_c);
socket_close($s_s);
?>
--CLEAN--
<?php
$path = sys_get_temp_dir() . "/socket_atmark.sock";
@unlink($path);
--EXPECT--
not OOB
