--TEST--
Dom\XMLDocument: failed write while serializing the document node must not detach its children
--EXTENSIONS--
dom
--FILE--
<?php

class FailingStream {
    public $context;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool {
        return true;
    }

    public function stream_write(string $data): int|false {
        return false;
    }

    public function stream_tell(): int {
        return 0;
    }
}

stream_wrapper_register('failing', FailingStream::class);

$doc = Dom\XMLDocument::createFromString('<root><child/></root>');
var_dump($doc->saveXmlFile('failing://out'));

var_dump($doc->documentElement?->nodeName);
var_dump($doc->childNodes->length);
echo $doc->saveXml();

?>
--EXPECT--
bool(false)
string(4) "root"
int(1)
<?xml version="1.0" encoding="UTF-8"?>
<root><child/></root>
