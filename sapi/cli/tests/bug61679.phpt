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

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

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
