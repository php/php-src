--TEST--
GH-8641 ([Stream] STREAM_NOTIFY_COMPLETED over HTTP never emitted)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

function stream_notification_callback($notification_code, $severity, $message, $message_code, $bytes_transferred, $bytes_max)
{
    if ($notification_code === STREAM_NOTIFY_COMPLETED) {
        echo $notification_code, ' ', $bytes_transferred, ' ', $bytes_max, PHP_EOL;
    }
}

$ctx = stream_context_create();
stream_context_set_params($ctx, array("notification" => "stream_notification_callback"));

$responses = array(
    "data://text/plain,HTTP/1.0 200 Ok\r\nContent-Length: 11\r\n\r\nHello world",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$f = file_get_contents($uri, 0, $ctx);

http_server_kill($pid);
var_dump($f);
?>
--EXPECTF--
8 11 11
string(11) "Hello world"
