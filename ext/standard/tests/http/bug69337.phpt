--TEST--
Bug #69337 (Stream context leaks when http request fails)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

function stream_notification_callback($notification_code, $severity, $message, $message_code, $bytes_transferred, $bytes_max)
{
    if($notification_code == STREAM_NOTIFY_REDIRECTED) {
      // $http_response_header is now a string, but will be used as an array
     // by php_stream_url_wrap_http_ex() later on
       $GLOBALS['http_response_header'] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\0\0\0\0";
    }
}

$ctx = stream_context_create();
stream_context_set_params($ctx, array("notification" => "stream_notification_callback"));

$responses = array(
    "data://text/plain,HTTP/1.0 302 Found\r\nLocation: /try-again\r\n\r\n",
    "data://text/plain,HTTP/1.0 404 Not Found\r\n\r\n",
);

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

$f = file_get_contents($uri, 0, $ctx);

http_server_kill($pid);
var_dump($f);
?>
--EXPECTF--
Warning: file_get_contents(http://%s:%d): Failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found%ain %s on line %d
bool(false)
