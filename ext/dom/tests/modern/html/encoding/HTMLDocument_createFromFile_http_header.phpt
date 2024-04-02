--TEST--
DOM\HTMLDocument::createFromFile() HTTP header Content-Type
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

$tests = [
    "No slashes" => [
        "foo",
        "      ",
    ],
    "Invalid type/subtype" => [
        "/html; Charset=\"ISO-8859-1\"",
        "text/; Charset=\"ISO-8859-1\"",
        "tex°t/html; Charset=\"ISO-8859-1\"",
        "/; Charset=\"ISO-8859-1\"",
        "$/€; Charset=\"ISO-8859-1\"",
        "; Charset=\"ISO-8859-1\"",
        ";",
        "",
        "    \t",
    ],
    "Valid type/subtype without charset" => [
        "text/html; x=ISO-8859-1",
        "text/html; x=\"ISO-8859-1\"",
        "text/html; charet=\"ISO-8859-1\"",
        "text/html; chars et=\"ISO-8859-1\"",
    ],
    "All valid inputs" => [
        "text/html; charset=ISO-8859-1",
        "\t\r text/html; charset=ISO-8859-1   \t",
        "\t\r text/html; charset=ISO-8859-1   \t;bar=\"foo\"",
        "\t\r text/html; charset=ISO-8859-1   \t;bar=\"foo\"\r\n\t ",
        "text/html; foo=bar;charset=ISO-8859-1",
        "text/html; foo=bar;charset=ISO-8859-1;bar=\"foooooo\"",
        "text/html;;;; charset=ISO-8859-1",
        "text/html; Charset=\"ISO-8859-1\"",
        "text/html; Charset=\"ISO\\-8859-1\"",
        "text/html; ;;   ; ;; Charset=\"ISO-8859-1\"",
        "text/html;Charset=\"ISO-8859-1",
        "tex.t/h#\$%!&'*%2B-.^_`|~tml;Charset=\"ISO-8859-1\"", // Note: have to encode + as 2B because of implementation details of http_server()
    ],
    "Valid input, but invalid encoding name" => [
        "text/html;Charset=\"ISO-8859-1\\",
        "text/html;Charset=\"ISO-8859-1\\\"",
        "text/html;Charset=\"foobar\\\"",
        "text/html;Charset=\"%7F\\\"",
        "text/html;Charset=\"\\\"",
        "text/html;Charset=",
    ],
];

foreach ($tests as $name => $headers) {
    echo "--- $name ---\n";
    $responses = array_map(fn ($header) => "data://text/plain,HTTP/1.1 200 OK\r\nContent-Type: " . $header . "\r\n\r\n" . "<p>\xE4\xF6\xFC</p>\n", $headers);
    ['pid' => $pid, 'uri' => $uri] = http_server($responses);
    for ($i = 0; $i < count($responses); $i++) {
        $result = DOM\HTMLDocument::createFromFile($uri, LIBXML_NOERROR);
        echo $result->getElementsByTagName("p")[0]->textContent, "\n";
    }
    http_server_kill($pid);
}
?>
--EXPECT--
--- No slashes ---
���
���
--- Invalid type/subtype ---
���
���
���
���
���
���
���
���
���
--- Valid type/subtype without charset ---
���
���
���
���
--- All valid inputs ---
äöü
äöü
äöü
äöü
äöü
äöü
äöü
äöü
äöü
äöü
äöü
äöü
--- Valid input, but invalid encoding name ---
���
���
���
���
���
���
