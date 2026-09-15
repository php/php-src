--TEST--
gh22981: connecting to a unix socket with a full listen backlog
--SKIPIF--
<?php
if (!is_callable('proc_open')) die('skip proc_open() is not available');
/* only Linux reports EAGAIN for a connect() to a unix socket with a full
   listen backlog; other systems fail with a hard error instead */
if (PHP_OS !== 'Linux') die('skip requires EAGAIN on a full unix socket backlog');
$path = sys_get_temp_dir() . '/gh22981_skip.sock';
@unlink($path);
$server = @stream_socket_server('unix://' . $path, $errno, $errstr);
if (!$server) die('skip unix domain sockets are not supported');
fclose($server);
@unlink($path);
--FILE--
<?php
$socketPath = sys_get_temp_dir() . '/gh22981_' . getmypid() . '.sock';
$stuckPath = sys_get_temp_dir() . '/gh22981_' . getmypid() . 'b.sock';
$procs = [];

register_shutdown_function(function () use ($socketPath, $stuckPath, &$procs) {
    foreach ($procs as $proc) {
        if (is_resource($proc[0])) {
            fclose($proc[1]);
            proc_terminate($proc[0]);
            proc_close($proc[0]);
        }
    }
    @unlink($socketPath);
    @unlink($stuckPath);
});

if (($argv[1] ?? '') === 'server' || ($argv[1] ?? '') === 'stuck') {
    $path = $argv[2];
    $ctx = stream_context_create(['socket' => ['backlog' => 1]]);
    $server = stream_socket_server('unix://' . $path, $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    if (!$server) {
        fwrite(STDERR, "server: $errstr\n");
        exit(1);
    }
    fwrite(STDOUT, "ready\n");
    if ($argv[1] === 'stuck') {
        /* never accept anything */
        sleep(3);
        exit(0);
    }
    /* let the clients pile up in the listen backlog before accepting */
    usleep(200000);
    $end = microtime(true) + 5;
    for ($accepted = 0; $accepted < 20 && microtime(true) < $end;) {
        $conn = @stream_socket_accept($server, 0.1);
        if ($conn) {
            fwrite($conn, "pong\n");
            $accepted++;
        }
    }
    exit(0);
}

/* the server starts accepting only after a delay: everyone has to wait for
   it, but nobody fails */
$proc = proc_open([PHP_BINARY, __FILE__, 'server', $socketPath],
    [1 => ['pipe', 'w']], $pipes);
if (!is_resource($proc)) {
    echo "cannot start server\n";
    exit(1);
}
$procs[] = [$proc, $pipes[1]];
fgets($pipes[1]); /* wait until the server is listening */

/* listen(1) only leaves room for a couple of pending connections, so most
   of these have to wait for the server to accept */
$conns = [];
for ($i = 0; $i < 20; $i++) {
    $conn = stream_socket_client('unix://' . $socketPath, $errno, $errstr, 5);
    if (!$conn) {
        printf("connect #%d failed: %s\n", $i + 1, $errstr);
        exit(1);
    }
    $conns[] = $conn;
}

if (trim((string) fgets($conns[count($conns) - 1])) !== 'pong') {
    echo "no pong\n";
    exit(1);
}

echo "ok\n";

/* the server never accepts: the connect timeout has to be honoured */
$proc = proc_open([PHP_BINARY, __FILE__, 'stuck', $stuckPath],
    [1 => ['pipe', 'w']], $pipes);
if (!is_resource($proc)) {
    echo "cannot start stuck server\n";
    exit(1);
}
$procs[] = [$proc, $pipes[1]];
fgets($pipes[1]); /* wait until the server is listening */

for ($i = 0; $i < 2; $i++) {
    $conn = stream_socket_client('unix://' . $stuckPath, $errno, $errstr, 3);
    if (!$conn) {
        printf("fill #%d failed: %s\n", $i + 1, $errstr);
        exit(1);
    }
    $conns[] = $conn;
}

$t = microtime(true);
$conn = @stream_socket_client('unix://' . $stuckPath, $errno, $errstr, 0.5);
$elapsed = microtime(true) - $t;

if ($conn) {
    echo "connect unexpectedly succeeded\n";
} elseif ($elapsed < 0.45 || $elapsed > 2.5) {
    printf("connect timeout not honoured: %.2fs (%s)\n", $elapsed, $errstr);
} elseif (strpos($errstr, 'timed out') === false) {
    printf("unexpected error: %s\n", $errstr);
} else {
    echo "timeout ok\n";
}
--EXPECT--
ok
timeout ok
