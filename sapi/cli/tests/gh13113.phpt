--TEST--
GH-13113: Missing syntax error in CLI-server router script
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('foo bar');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();
$request = <<<REQUEST
GET / HTTP/1.1
Host: $host


REQUEST;

if(fwrite($fp, $request)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

<br />
<b>Parse error</b>:  syntax error, unexpected identifier &quot;bar&quot; in <b>%sindex.php</b> on line <b>1</b><br />
