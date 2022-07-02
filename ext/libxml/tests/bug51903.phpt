--TEST--
Bug #51903 (simplexml_load_file() doesn't use HTTP headers)
--EXTENSIONS--
simplexml
--SKIPIF--
<?php
if (@!include "./ext/standard/tests/http/server.inc") die('skip server.inc not available');
http_server_skipif();
?>
--FILE--
<?php
require "./ext/standard/tests/http/server.inc";
$responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\n"
    . "Content-Type: text/xml; charset=ISO-8859-1\r\n\r\n"
    . "<?xml version=\"1.0\"?>\n"
    . "<root>\xE4\xF6\xFC</root>\n",
    "data://text/plain,HTTP/1.1 200 OK\r\n"
    . "Content-Type: text/xml; charset=ISO-8859-1; foo=bar\r\n\r\n"
    . "<?xml version=\"1.0\"?>\n"
    . "<root>\xE4\xF6\xFC</root>\n",
    "data://text/plain,HTTP/1.1 200 OK\r\n"
    . "Content-Type: text/xml; charset=\"ISO-8859-1\" ; foo=bar\r\n\r\n"
    . "<?xml version=\"1.0\"?>\n"
    . "<root>\xE4\xF6\xFC</root>\n",
];
['pid' => $pid, 'uri' => $uri] = http_server($responses);

for ($i = 0; $i < count($responses); $i++) {
    $sxe = simplexml_load_file($uri);
    echo "$sxe\n";
}

http_server_kill($pid);
?>
--EXPECT--
äöü
äöü
äöü
