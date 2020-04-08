--TEST--
No router, no script
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(NULL, NULL);

$output = '';

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
POST / HTTP/1.1
Host: {$host}
Content-Type: application/x-www-form-urlencoded
Content-Length: 3

a=b
HEADER
)) {
	while (!feof($fp)) {
		$output .= fgets($fp);
	}
}

echo preg_replace("/<style>(.*?)<\/style>/s", "<style>AAA</style>", $output), "\n";
fclose($fp);


$output = '';
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET /main/style.css HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$output .= fgets($fp);
	}
}

echo preg_replace("/<style>(.*?)<\/style>/s", "<style>AAA</style>", $output), "\n";
fclose($fp);

$output = '';
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
HEAD /main/foo/bar HTTP/1.1
Host: {$host}


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
HTTP/1.1 404 Not Found
Host: %s
Date: %s
Connection: close
Content-Type: text/html; charset=UTF-8
Content-Length: %d

<!doctype html><html><head><title>404 Not Found</title><style>AAA</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/</code> was not found on this server.</p></body></html>
HTTP/1.1 404 Not Found
Host: %s
Date: %s
Connection: close
Content-Type: text/html; charset=UTF-8
Content-Length: %d

<!doctype html><html><head><title>404 Not Found</title><style>AAA</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/main/style.css</code> was not found on this server.</p></body></html>
HTTP/1.1 404 Not Found
Host: %s
Date: %s
Connection: close
Content-Type: text/html; charset=UTF-8
Content-Length: %d

<!doctype html><html><head><title>404 Not Found</title><style>AAA</style>
</head><body><h1>Not Found</h1><p>The requested resource <code class="url">/main/foo/bar</code> was not found on this server.</p></body></html>
