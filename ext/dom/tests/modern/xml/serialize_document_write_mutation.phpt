--TEST--
Dom\XMLDocument: mutating the document from a stream write during document serialization
--EXTENSIONS--
dom
--FILE--
<?php

class MutatingStream {
    public $context;
    public static $doc;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool {
        return true;
    }

    public function stream_write(string $data): int {
        if (self::$doc !== null) {
            $doc = self::$doc;
            self::$doc = null;
            $doc->append($doc->createComment('injected'));
        }
        return strlen($data);
    }
}

stream_wrapper_register('mutating', MutatingStream::class);

$doc = Dom\XMLDocument::createFromString('<root><child/></root>');
MutatingStream::$doc = $doc;
$doc->saveXmlFile('mutating://out');

$doc->append($doc->createComment('after'));
var_dump($doc->childNodes->length);
echo $doc->saveXml(), "\n";

?>
--EXPECT--
int(3)
<?xml version="1.0" encoding="UTF-8"?>
<root><child/></root><!--injected--><!--after-->
