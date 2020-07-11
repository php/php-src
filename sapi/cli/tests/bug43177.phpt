--TEST--
Bug #61977 Test exit code for various errors
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'SCRIPT'
    ini_set('display_errors', 0);
    switch($_SERVER["REQUEST_URI"]) {
            case "/parse":
                    try {
                        eval("this is a parse error");
                    } catch (ParseError $e) {
                    }
                    echo "OK\n";
                    break;
            case "/fatal":
                    eval("foo();");
                    echo "OK\n";
                    break;
            case "/compile":
                    eval("class foo { final private final function bar() {} }");
                    echo "OK\n";
                    break;
            case "/fatal2":
                    foo();
                    echo "OK\n";
                    break;
            default:
                    return false;
    }
SCRIPT
);

$host = PHP_CLI_SERVER_HOSTNAME;

foreach(array("parse", "fatal", "fatal2", "compile") as $url) {
    $fp = php_cli_server_connect();
    if(fwrite($fp, <<<HEADER
GET /$url HTTP/1.1
Host: {$host}


HEADER
)) {
        while (!feof($fp)) {
                echo fgets($fp);
        }
    }
}

?>
--EXPECTF--
HTTP/1.1 200 OK
Host: localhost
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

OK
HTTP/1.0 500 Internal Server Error
Host: localhost
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

HTTP/1.0 500 Internal Server Error
Host: localhost
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

HTTP/1.0 500 Internal Server Error
Host: localhost
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8
