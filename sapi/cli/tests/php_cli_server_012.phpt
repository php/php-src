--TEST--
Bug #60159 (Router returns false, but POST is not passed to requested resource)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('print_r($_REQUEST); $_REQUEST["foo"] = "bar"; return FALSE;');
$doc_root = __DIR__;
file_put_contents($doc_root . '/request.php', '<?php print_r($_REQUEST); ?>');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
POST /request.php HTTP/1.1
Host: {$host}
Content-Type: application/x-www-form-urlencoded
Content-Length: 3

a=b
HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);
@unlink($doc_root . '/request.php');

?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

Array
(
    [a] => b
)
Array
(
    [a] => b
    [foo] => bar
)
