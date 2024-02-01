--TEST--
Bug #60477: Segfault after two multipart/form-data POST requests
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('echo "done\n";', null);

$output = '';
$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
POST /index.php HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=---------123456789
Content-Length: 70

---------123456789
Content-Type: application/x-www-form-urlencoded
a=b
HEADER
)) {
    while (!feof($fp)) {
        $output .= fgets($fp);
    }
}

fclose($fp);

$fp = php_cli_server_connect();
if(fwrite($fp, <<<HEADER
POST /main/no-exists.php HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=---------123456789
Content-Length: 70

---------123456789
Content-Type: application/x-www-form-urlencoded
a=b
HEADER
)) {
    while (!feof($fp)) {
        $output .= fgets($fp);
    }
}

echo preg_replace("/<style>(.*?)<\/style>/s", "<style>AAA</style>", $output), "\n";
fclose($fp);

?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: %s
Content-type: %s

done
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: %s

done
