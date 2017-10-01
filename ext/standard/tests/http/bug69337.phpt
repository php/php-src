--TEST--
Bug #69337 (Stream context leaks when http request fails)
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:22345'); ?>
--INI--
allow_url_fopen=1
allow_url_include=1
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
	"data://text/plain,HTTP/1.0 302 Found\r\nLocation: http://127.0.0.1:22345/try-again\r\n\r\n",
	"data://text/plain,HTTP/1.0 404 Not Found\r\n\r\n",
);

$pid = http_server("tcp://127.0.0.1:22345", $responses, $output);

$f = file_get_contents('http://127.0.0.1:22345/', 0, $ctx);

http_server_kill($pid);
var_dump($f);
?>
==DONE==
--EXPECTF--
Warning: file_get_contents(http://127.0.0.1:22345/): failed to open stream: HTTP request failed! HTTP/1.0 404 Not Found%ain %s on line %d
bool(false)
==DONE==
