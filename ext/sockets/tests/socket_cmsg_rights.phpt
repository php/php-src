--TEST--
recvmsg(): receive SCM_CREDENTIALS messages
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
?>
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$path = sys_get_temp_dir() . "/socket_cmsg_rights.sock";

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

$r = socket_sendmsg($sends1, [
  "name" => [ "path" => $path ],
  "iov" => ["test ", "thing", "\n"],
  "control" => [
    [
      "level" => SOL_SOCKET,
      "type" => SCM_RIGHTS,
      "data" => [$sends1, STDIN, STDOUT, STDERR],
    ]
  ]
], 0);
var_dump($r);
checktimeout($s, 500);

$data = [
    "name" => [],
    "buffer_size" => 2000,
    "controllen" => socket_cmsg_space(SOL_SOCKET, SCM_RIGHTS, 4)
];
var_dump($data);
if (!socket_recvmsg($s, $data, 0)) die("recvmsg");

if ($data["control"]) {
    $control = $data["control"][0];
    if ($control["level"] == SOL_SOCKET &&
        $control["type"]  == SCM_RIGHTS) {
        foreach ($control["data"] as $resource) {
            if (!is_resource($resource)) {
                echo "FAIL RES\n";
                var_dump($data);
                exit;
            }
        }
        echo "OK";
    } else {
        echo "FAIL RIGHTS\n";
        var_dump($data);
        exit;
    }
} else {
    echo "FAIL CONTROL\n";
    var_dump($data);
}
?>
--CLEAN--
<?php
$path = sys_get_temp_dir() . "/socket_cmsg_rights.sock";
@unlink($path);
?>
--EXPECTF--
creating send socket
object(Socket)#%d (0) {
}
creating receive socket
object(Socket)#%d (0) {
}
bool(true)
int(11)
array(3) {
  ["name"]=>
  array(0) {
  }
  ["buffer_size"]=>
  int(2000)
  ["controllen"]=>
  int(%d)
}
OK
