--TEST--
Bug #47021 (SoapClient stumbles over WSDL delivered with "Transfer-Encoding: chunked")
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

function stream_notification_callback($notification_code, $severity, $message, $message_code, $bytes_transferred, $bytes_max) {

    switch($notification_code) {
        case STREAM_NOTIFY_MIME_TYPE_IS:
            echo "Type='$message'\n";
	    break;
        case STREAM_NOTIFY_FILE_SIZE_IS:
            echo "Size=$bytes_max\n";
            break;
    }
}

function do_test($num_spaces, $leave_trailing_space=false) {
  // SOAPClient exhibits the bug because it forces HTTP/1.1,
  // whereas file_get_contents() uses HTTP/1.0 by default.
  $options = [
    'http' => [
      'protocol_version' => '1.1',
      'header' => 'Connection: Close'
    ],
  ];

  $ctx = stream_context_create($options);
  stream_context_set_params($ctx, array("notification" => "stream_notification_callback"));

  $spaces = str_repeat(' ', $num_spaces);
  $trailing = ($leave_trailing_space ? ' ' : '');
  $responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\n"
      . "Content-Type:{$spaces}text/plain{$trailing}\r\n"
      . "Transfer-Encoding:{$spaces}Chunked{$trailing}\r\n\r\n"
      . "5\nHello\n0\n",
    "data://text/plain,HTTP/1.1 200 OK\r\n"
      . "Content-Type\r\n" // Deliberately invalid header
      . "Content-Length:{$spaces}5{$trailing}\r\n\r\n"
      . "World"
  ];
  $pid = http_server('tcp://127.0.0.1:12342', $responses);

  echo file_get_contents('http://127.0.0.1:12342/', false, $ctx);
  echo "\n";
  echo file_get_contents('http://127.0.0.1:12342/', false, $ctx);
  echo "\n";

  http_server_kill($pid);
}

// Chunked decoding should be recognised by the HTTP stream wrapper regardless of whitespace
// Transfer-Encoding:Chunked
do_test(0);
echo "\n";
// Transfer-Encoding: Chunked
do_test(1);
echo "\n";
// Transfer-Encoding:  Chunked
do_test(2);
echo "\n";
// Trailing space at end of header
do_test(1, true);
echo "\n";

?>
--EXPECT--
Type='text/plain'
Hello
Size=5
World

Type='text/plain'
Hello
Size=5
World

Type='text/plain'
Hello
Size=5
World

Type='text/plain'
Hello
Size=5
World

