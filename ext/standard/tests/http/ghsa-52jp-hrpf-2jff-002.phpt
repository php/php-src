--TEST--
GHSA-52jp-hrpf-2jff: HTTP stream wrapper truncate redirect location to 1024 bytes (over limit)
--FILE--
<?php
$serverCode = <<<'CODE'
$ctxt = stream_context_create([
     "socket" => [
         "tcp_nodelay" => true
     ]
 ]);

 $server = stream_socket_server(
     "tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctxt);
 phpt_notify_server_start($server);

 $conn = stream_socket_accept($server);

 phpt_notify(message:"server-accepted");

 $loc = str_repeat("y", 9000);
 fwrite($conn, "HTTP/1.0 301 Ok\r\nContent-Type: text/html;\r\nLocation: $loc\r\n\r\nbody\r\n");
CODE;

$clientCode = <<<'CODE'
 function stream_notification_callback($notification_code, $severity, $message, $message_code, $bytes_transferred, $bytes_max) {
 switch($notification_code) {
     case STREAM_NOTIFY_MIME_TYPE_IS:
         echo "Found the mime-type: ", $message, PHP_EOL;
         break;
     case STREAM_NOTIFY_REDIRECTED:
         echo "Redirected: ";
         var_dump($message);
     }
 }

 $ctx = stream_context_create();
 stream_context_set_params($ctx, array("notification" => "stream_notification_callback"));
 var_dump(trim(file_get_contents("http://{{ ADDR }}", false, $ctx)));
 var_dump(http_get_last_response_headers());
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
Found the mime-type: text/html;

Warning: file_get_contents(http://127.0.0.1:%d): Failed to open stream: HTTP Location header size is over the limit of 8182 bytes in %s
string(0) ""
array(2) {
  [0]=>
  string(15) "HTTP/1.0 301 Ok"
  [1]=>
  string(24) "Content-Type: text/html;"
}
