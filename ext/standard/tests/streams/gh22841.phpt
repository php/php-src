--TEST--
GH-22841 (php_stream_copy_to_stream_ex() drops progress notifications)
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip proc_open() is not available");
?>
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php

/* A 43 byte gzip stream decompressing to "Hello World!\n". Kept as a literal so
 * that the server, which is started with -n, does not need zlib itself. */
$serverCode = <<<'CODE'
$data = base64_decode("H4sICPQfX2oAA2luZGV4LnBocADzSM3JyVcIzy/KSVHkAgDd3RR9DQAAAA==");
header("Content-Type: application/gzip");
header("Content-Length: " . strlen($data));
echo $data;
CODE;

include __DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$names = [
    STREAM_NOTIFY_CONNECT => "CONNECT",
    STREAM_NOTIFY_MIME_TYPE_IS => "MIME_TYPE_IS",
    STREAM_NOTIFY_FILE_SIZE_IS => "FILE_SIZE_IS",
    STREAM_NOTIFY_PROGRESS => "PROGRESS",
    STREAM_NOTIFY_COMPLETED => "COMPLETED",
];

$events = [];
$context = stream_context_create();
stream_context_set_params($context, ["notification" =>
    function ($code, $severity, $message, $message_code, $bytes_transferred, $bytes_max)
    use (&$events, $names) {
        $name = $names[$code] ?? $code;
        $event = $name . " " . ($code === STREAM_NOTIFY_FILE_SIZE_IS ? $bytes_max : $bytes_transferred);
        /* The body may arrive in more than one read, so collapse consecutive
         * progress events into the latest one to keep the output stable. */
        if ($name === "PROGRESS" && str_starts_with(end($events) ?: "", "PROGRESS ")) {
            array_pop($events);
        }
        $events[] = $event;
    }
]);

/* compress.zlib:// opens the inner stream with STREAM_MUST_SEEK, which copies
 * the HTTP stream into a temporary file through php_stream_copy_to_stream_ex(). */
$fp = fopen("compress.zlib://http://" . PHP_CLI_SERVER_ADDRESS . "/", "r", false, $context);
var_dump(stream_get_contents($fp));
fclose($fp);

echo implode(PHP_EOL, $events), PHP_EOL;

?>
--EXPECT--
string(13) "Hello World!
"
CONNECT 0
MIME_TYPE_IS 0
FILE_SIZE_IS 43
PROGRESS 43
COMPLETED 43
