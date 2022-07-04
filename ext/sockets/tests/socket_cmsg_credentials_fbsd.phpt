--TEST--
recvmsg(): receive SCM_CREDS messages
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!str_contains(PHP_OS, 'FreeBSD')) {
    die('skipped supported only on FreeBSD');
}
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$path = __DIR__ . "/socket_cmsg_credentials_fbsd.sock";

@unlink($path);

echo "creating send socket\n";
$sends1 = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("err");
var_dump($sends1);
socket_set_nonblock($sends1) or die("Could not put in non-blocking mode");

echo "creating receive socket\n";
$s = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("err");
var_dump($s);
$br = socket_bind($s, $path) or die("err");
var_dump($br);
socket_set_nonblock($s) or die("Could not put in non-blocking mode");
socket_set_option($s, SOL_LOCAL, LOCAL_CREDS_PERSISTENT, 1) or die("could not set LOCAL_CREDS");


$r = socket_sendto($sends1, $msg = "dread", strlen($msg), 0, $path);
var_dump($r);
checktimeout($s, 500);

$data = [
    "name" => [],
    "buffer_size" => 2000,
    "controllen" => socket_cmsg_space(SOL_SOCKET, SCM_CREDS2, 1)
];
if (!socket_recvmsg($s, $data, 0)) die("recvmsg");
print_r($data);

$pid = getmypid();
var_dump($data['control'][0]['data']['pid'] === $pid);
?>
--CLEAN--
<?php
$path = __DIR__ . "/socket_cmsg_credentials_fbsd.sock";
@unlink($path);
--EXPECTF--
creating send socket
object(Socket)#%d (0) {
}
creating receive socket
object(Socket)#%d (0) {
}
bool(true)
int(5)
Array
(
    [name] => Array
        (
            [family] => %d
            [path] => 
        )

    [control] => Array
        (
            [0] => Array
                (
                    [level] => %d
                    [type] => %d
                    [data] => Array
                        (
                            [pid] => %d
                            [uid] => %d
                            [gid] => %d
                        )

                )

        )

    [iov] => Array
        (
            [0] => dread
        )

    [flags] => 0
)
bool(true)
