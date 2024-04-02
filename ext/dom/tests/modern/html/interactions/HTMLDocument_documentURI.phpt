--TEST--
DOM\HTMLDocument::documentURI
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__ . "/test foo.html", LIBXML_NOERROR);
var_dump($dom->documentURI);

$memory = fopen("php://memory", "w+");
fwrite($memory, "foobar");
rewind($memory);
$dom = DOM\HTMLDocument::createFromFile("php://memory");
var_dump($dom->documentURI);
fclose($memory);

class DummyWrapper {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_read($count) {
        return "";
    }

    public function stream_eof() {
        return true;
    }

    public function stream_close() {
        return true;
    }
}

stream_wrapper_register("dummy", DummyWrapper::class);

$dom = DOM\HTMLDocument::createFromFile("dummy://foo/ bar");
var_dump($dom->documentURI);

?>
--EXPECTF--
string(%d) "file://%stest%20foo.html"
string(12) "php://memory"
string(16) "dummy://foo/ bar"
