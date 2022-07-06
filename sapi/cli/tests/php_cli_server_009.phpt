--TEST--
PATH_INFO (relevant to #60112)
--DESCRIPTION--
After this fix(#60112), previously 404 request like "localhost/foo/bar"
now could serve correctly with request_uri "index.php" and PATH_INFO "/foo/bar/"
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["PATH_INFO"]);', null);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET /foo/bar HTTP/1.1
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);

$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET /foo/bar/ HTTP/1.0
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);

$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET /foo/bar.js HTTP/1.0
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
        break;
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

string(8) "/foo/bar"
HTTP/1.0 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(9) "/foo/bar/"
HTTP/1.0 404 Not Found
