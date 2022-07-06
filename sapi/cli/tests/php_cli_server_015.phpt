--TEST--
Bug #60523 (PHP Errors are not reported in browsers using built-in SAPI)
--SKIPIF--
<?php
include "skipif.inc";
?>
--INI--
display_errors=1
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('require("syntax_error.php");');
$dir = realpath(__DIR__);

file_put_contents($dir . "/syntax_error.php", "<?php non_exists_function(); ?>");

$output = '';
$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET /index.php HTTP/1.1
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        $output .= fgets($fp);
    }
}
echo $output;
@unlink($dir . "/syntax_error.php");
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
<b>Fatal error</b>:  Uncaught Error: Call to undefined function non_exists_function() in %ssyntax_error.php:%d
Stack trace:
#0 %sindex.php(%d): require()
#1 {main}
  thrown in <b>%ssyntax_error.php</b> on line <b>%d</b><br />
