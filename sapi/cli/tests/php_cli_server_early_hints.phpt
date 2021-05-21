--TEST--
PHP CLI server HTTP early hints
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
    header('HTTP/1.1 103 Early Hints');
    header('Link: </style.css>; rel=preload; as=style');
    headers_send_early_and_clear();
    header('Location: http://example.com/');
    echo "Foo\n";
    PHP);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if (fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}


HEADER
)) {
	fpassthru($fp);
}

?>
--EXPECTF--
HTTP/1.1 103 Early Hints
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Link: </style.css>; rel=preload; as=style

HTTP/1.1 302 Found
Host: localhost
Date: %s
Connection: close
Location: http://example.com/
Content-type: text/html; charset=UTF-8

Foo
