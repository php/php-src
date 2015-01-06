--TEST--
Bug #61977 test CLI web-server support for Mime Type File extensions mapping
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('<?php ?>', null);

/*
 * If a Mime Type is added in php_cli_server.c, add it to this array and update
 * the EXPECTF section accordingly
 */
$mimetypes = ['html', 'htm', 'svg', 'css', 'js', 'png', 'webm', 'ogv', 'ogg'];

function test_mimetypes($mimetypes) {
    foreach ($mimetypes as $mimetype) {
        list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
        $port = intval($port) ? : 80;
        $fp   = fsockopen($host, $port, $errno, $errstr, 0.5);
        if (!$fp) die('Connect failed');
        file_put_contents(__DIR__ . "/foo.{$mimetype}", '');
        $header = <<<HEADER
GET /foo.{$mimetype} HTTP/1.1
Host: {$host}


HEADER;
        if (fwrite($fp, $header)) {
            while (!feof($fp)) {
                $text = fgets($fp);
                if (strncasecmp("Content-type:", $text, 13) == 0) {
                    echo "foo.{$mimetype} => ", $text;
                }
            }
            @unlink(__DIR__ . "/foo.{$mimetype}");
            fclose($fp);
        }
    }
}

test_mimetypes($mimetypes);
?>
--EXPECTF--
foo.html => Content-Type: text/html; charset=UTF-8
foo.htm => Content-Type: text/html; charset=UTF-8
foo.svg => Content-Type: image/svg+xml
foo.css => Content-Type: text/css; charset=UTF-8
foo.js => Content-Type: text/javascript; charset=UTF-8
foo.png => Content-Type: image/png
foo.webm => Content-Type: video/webm
foo.ogv => Content-Type: video/ogg
foo.ogg => Content-Type: audio/ogg
