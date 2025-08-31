--TEST--
GH-10031 ([Stream] STREAM_NOTIFY_PROGRESS over HTTP emitted irregularly for last chunk of data)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php

$serverCode = <<<'CODE'
$fsize = 1000;
$chunksize = 99;
$chunks = floor($fsize / $chunksize); // 10 chunks * 99 bytes
$lastchunksize = $fsize - $chunksize * $chunks; // 1 chunk * 10 bytes

header("Content-Length: " . $fsize);
flush();
for ($chunk = 1; $chunk <= $chunks; $chunk++) {
    echo str_repeat('x', $chunksize);
    @ob_flush();
    usleep(50 * 1000);
}

echo str_repeat('x', $lastchunksize);
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$context = stream_context_create(['http' => ['ignore_errors' => true,]]);
$lastBytesTransferred = 0;
stream_context_set_params($context, ['notification' => function ($code, $s, $m, $mc, $bytes_transferred, $bytes_max)
use (&$lastBytesTransferred) {
    if ($code === STREAM_NOTIFY_FILE_SIZE_IS) echo "expected filesize=$bytes_max".PHP_EOL;
    $lastBytesTransferred = $bytes_transferred;
    @ob_flush();
}]);

$get = file_get_contents("http://".PHP_CLI_SERVER_ADDRESS, false, $context);

echo "got filesize=" . strlen($get) . PHP_EOL;
var_dump($lastBytesTransferred);

?>
--EXPECT--
expected filesize=1000
got filesize=1000
int(1000)
