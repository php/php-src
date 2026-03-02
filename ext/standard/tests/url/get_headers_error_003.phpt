--TEST--
Test get_headers() function : test with context
--FILE--
<?php

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start('header("X-Request-Method: ".$_SERVER["REQUEST_METHOD"]);');

$opts = array(
  'http' => array(
    'method' => 'HEAD'
  )
);

$context = stream_context_create($opts);
$headers = get_headers("http://".PHP_CLI_SERVER_ADDRESS, 1, $context);
echo $headers["X-Request-Method"]."\n";

stream_context_set_default($opts);
$headers = get_headers("http://".PHP_CLI_SERVER_ADDRESS, 1);
echo $headers["X-Request-Method"]."\n";

echo "Done";
?>
--EXPECT--
HEAD
HEAD
Done
