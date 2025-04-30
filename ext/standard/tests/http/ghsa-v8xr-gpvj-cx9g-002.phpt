--TEST--
GHSA-v8xr-gpvj-cx9g: Header parser of http stream wrapper does not handle folded headers (multiple)
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

    fwrite($conn, "HTTP/1.0 200 Ok\r\nContent-Type: text/html;\r\nCustom-Header: somevalue;\r\n  param1=value1; \r\n    param2=value2\r\n\r\nbody\r\n");
CODE;

$clientCode = <<<'CODE'
    function stream_notification_callback($notification_code, $severity, $message, $message_code, $bytes_transferred, $bytes_max) {
    switch($notification_code) {
        case STREAM_NOTIFY_MIME_TYPE_IS:
            echo "Found the mime-type: ", $message, PHP_EOL;
            break;
        }
    }

    $ctx = stream_context_create();
    stream_context_set_params($ctx, array("notification" => "stream_notification_callback"));
    var_dump(trim(file_get_contents("http://{{ ADDR }}", false, $ctx)));
    var_dump($http_response_header);
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
Found the mime-type: text/html;
string(4) "body"
array(3) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(24) "Content-Type: text/html;"
  [2]=>
  string(54) "Custom-Header: somevalue; param1=value1; param2=value2"
}
