--TEST--
GH-16883 (gzopen() does not use the default stream context when opening HTTP URLs)
--EXTENSIONS--
zlib
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php
require __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$code = <<<'PHP'
echo $_SERVER['HTTP_USER_AGENT'], "\n";
PHP;

php_cli_server_start($code);

stream_context_set_default([
	'http' => array(
		'user_agent' => 'dummy',
	)
]);

$f = gzopen('http://'.PHP_CLI_SERVER_HOSTNAME.':'.PHP_CLI_SERVER_PORT, 'r');
var_dump(stream_get_contents($f));

var_dump(gzfile('http://'.PHP_CLI_SERVER_HOSTNAME.':'.PHP_CLI_SERVER_PORT));

var_dump(readgzfile('http://'.PHP_CLI_SERVER_HOSTNAME.':'.PHP_CLI_SERVER_PORT));

?>
--EXPECT--
string(6) "dummy
"
array(1) {
  [0]=>
  string(6) "dummy
"
}
dummy
int(6)
