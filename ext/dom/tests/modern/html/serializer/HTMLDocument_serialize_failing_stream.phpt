--TEST--
Dom\HTMLDocument serialization with a failing stream
--EXTENSIONS--
dom
--FILE--
<?php

class FailingWrapper {
    public $context;
    public $fail = false;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_write(string $data): int {
        if ($this->fail) {
            throw new Error("fail");
        }
        $this->fail = true;
        var_dump($data);
        return strlen($data);
    }

    public function stream_eof() {
        return false;
    }

    public function stream_close() {
        return true;
    }
}

stream_wrapper_register("failing", "FailingWrapper");

$dom = Dom\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));
$dom->saveHtmlFile("failing://foo");

?>
--EXPECTF--
string(1) "<"

Fatal error: Uncaught Error: fail in %s:%d
Stack trace:
#0 [internal function]: FailingWrapper->stream_write('root')
#1 %s(%d): Dom\HTMLDocument->saveHtmlFile('failing://foo')
#2 {main}
  thrown in %s on line %d
