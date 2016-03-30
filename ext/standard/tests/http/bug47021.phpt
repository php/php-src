--TEST--
Bug #47021 (SoapClient stumbles over WSDL delivered with "Transfer-Encoding: chunked")
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif('tcp://127.0.0.1:12342'); ?>
--FILE--
<?php
require 'server.inc';

function do_test($num_spaces) {
  // SOAPClient exhibits the bug because it forces HTTP/1.1,
  // whereas file_get_contents() uses HTTP/1.0 by default.
  $options = [
    'http' => [
      'protocol_version' => '1.1',
      'header' => 'Connection: Close'
    ],
  ];

  $ctx = stream_context_create($options);

  $spaces = str_repeat(' ', $num_spaces);
  $responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\nTransfer-Encoding:{$spaces}Chunked\r\n\r\n5\nHello\n0\n"
  ];
  $pid = http_server('tcp://127.0.0.1:12342', $responses);

  echo file_get_contents('http://127.0.0.1:12342/', false, $ctx);

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

?>
--EXPECT--
Hello
Hello
Hello
