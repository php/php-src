--TEST--
Bug #61679 (Error on non-standard HTTP methods)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
echo "This should never echo";
PHP
);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

// Send a request with a fictitious request method,
// I like smurfs, the smurf everything.
if(fwrite($fp, <<<HEADER
SMURF / HTTP/1.1
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
        // Only echo the first line from the response,
        // the rest is not interesting
        break;
    }
}

fclose($fp);
?>
--EXPECT--
HTTP/1.1 501 Not Implemented
