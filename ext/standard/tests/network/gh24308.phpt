--TEST--
GH-24308 (syslog() without openlog() is closed at request end)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
if (!@is_dir('/proc/self/fd')) die('skip requires /proc/self/fd');
if (!@stream_socket_client('udg:///dev/log')) die('skip requires /dev/log');
?>
--FILE--
<?php
$serverCode = <<<'CODE'
function sockets() {
    $sockets = [];
    foreach (scandir('/proc/self/fd') as $fd) {
        $link = @readlink("/proc/self/fd/$fd");
        if ($link !== false && str_starts_with($link, 'socket:')) {
            $sockets[$fd] = $link;
        }
    }
    return $sockets;
}

if (isset($_GET['syslog'])) {
    $before = sockets();
    syslog(LOG_DEBUG, 'GH-24308');
    echo json_encode(array_diff_assoc(sockets(), $before));
} else {
    echo json_encode(sockets());
}
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$opened = json_decode(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/?syslog"), true);
var_dump(count($opened));

$next = json_decode(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/"), true);
var_dump(array_intersect_assoc($opened, $next));
?>
--EXPECT--
int(1)
array(0) {
}
