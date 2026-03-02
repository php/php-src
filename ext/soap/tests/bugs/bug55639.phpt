--TEST--
Bug #55639 (Digest authentication dont work)
--INI--
soap.wsdl_cache_enabled=0
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php

include __DIR__ . "/../../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
  // Necessary such that it works from a development directory in which case extension_dir might not be the real extension dir
  $args[] = "-c";
  $args[] = php_ini_loaded_file();
}

$code = <<<'PHP'
/* Receive */
header('HTTP/1.0 401 Unauthorized');
header('WWW-Authenticate: Digest realm="realm", qop="auth,auth-int", nonce="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", opaque="bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"');
file_get_contents("php://input");
PHP;

php_cli_server_start($code, null, $args);

$client = new soapclient(NULL, [
  'location' => 'http://' . PHP_CLI_SERVER_ADDRESS,
  'uri' => 'misc-uri',
  'authentication' => SOAP_AUTHENTICATION_DIGEST,
  'realm' => 'myrealm',
  'login' => 'user',
  'password' => 'pass',
  'trace' => true,
]);

try {
    $client->__soapCall("foo", []);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

$headers = $client->__getLastRequestHeaders();
var_dump($headers);

?>
--EXPECTF--
Unauthorized
string(%d) "POST / HTTP/1.1
Host: %s
Connection: Keep-Alive
User-Agent: %s
Content-Type: text/xml; charset=utf-8
SOAPAction: "misc-uri#foo"
Content-Length: %d
Authorization: Digest username="user", realm="realm", nonce="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", uri="/", qop=auth, nc=00000001, cnonce="%s", response="%s", opaque="bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"

"
