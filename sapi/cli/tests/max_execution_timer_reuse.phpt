--TEST--
Zend max execution timers keep firing when reused across requests on one thread
--ENV--
PHP_CLI_SERVER_WORKERS=
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
if (PHP_OS !== 'Linux') {
    die("skip Linux only");
}
ob_start();
phpinfo(INFO_GENERAL);
if (!str_contains(ob_get_clean(), 'Zend Max Execution Timers => enabled')) {
    die("skip Zend max execution timers are disabled");
}
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
set_time_limit(1);
if (isset($_GET['busy'])) {
    while (true) {}
}
echo "ok\n";
PHP);

/* A broken timer would leave the busy loop spinning; fail instead of hanging. */
$context = stream_context_create(['http' => ['ignore_errors' => true, 'timeout' => 10]]);
foreach (['busy', 'idle', 'busy'] as $request) {
    $query = $request === 'busy' ? '?busy=1' : '';
    $body = @file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/$query", false, $context);
    if ($body === false) {
        echo "$request: no response\n";
    } elseif (preg_match('/Maximum execution time of 1 second exceeded/', $body)) {
        echo "$request: timed out\n";
    } else {
        echo "$request: ", trim($body), "\n";
    }
}
?>
--EXPECT--
busy: timed out
idle: ok
busy: timed out
