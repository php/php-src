--TEST--
GHSA-p3x9-6h7p-cgfc: libxml streams use wrong `content-type` header when requesting a redirected resource (Reason with colon)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (@!include "./ext/standard/tests/http/server.inc") die('skip server.inc not available');
http_server_skipif();
?>
--FILE--
<?php
require "./ext/standard/tests/http/server.inc";

function genResponses($server) {
    $uri = 'http://' . stream_socket_get_name($server, false);
    yield "data://text/plain,HTTP/1.1 302 Moved Temporarily\r\nLocation: $uri/document.xml\r\nContent-Type: text/html;charset=utf-16\r\n\r\n";
    $xml = <<<'EOT'
        <!doctype html>
        <html>
            <head>
                <title>GHSA-p3x9-6h7p-cgfc</title>

                <meta charset="utf-8" />
                <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
            </head>

            <body>
                <h1>GHSA-p3x9-6h7p-cgfc</h1>
            </body>
        </html>
        EOT;
    // Missing content-type in actual response.
    yield "data://text/plain,HTTP/1.1 200 OK: This is fine\r\n\r\n{$xml}";
}

['pid' => $pid, 'uri' => $uri] = http_server('genResponses', $output);
$document = new \DOMDocument();
$document->loadHTMLFile($uri);

$h1 = $document->getElementsByTagName('h1');
var_dump($h1->length);
var_dump($document->saveHTML());
http_server_kill($pid);
?>
--EXPECT--
int(1)
string(266) "<!DOCTYPE html>
<html>
    <head>
        <title>GHSA-p3x9-6h7p-cgfc</title>

        <meta charset="utf-8">
        <meta http-equiv="Content-type" content="text/html; charset=utf-8">
    </head>

    <body>
        <h1>GHSA-p3x9-6h7p-cgfc</h1>
    </body>
</html>
"
