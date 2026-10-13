--TEST--
XMLReader under io_hooks: the reader refuses other use while a read waits for input
--EXTENSIONS--
xmlreader
dom
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

[$writer, $input] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
fwrite($writer, '<?xml version="1.0"?>');
$reader = XMLReader::fromStream($input);

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use (&$reader) {
    var_dump($reader->read());
    echo "A: done\n";
});

$scheduler->spawn(function () use (&$reader, $writer) {
    $calls = [
        'close' => fn() => $reader->close(),
        'read' => fn() => $reader->read(),
        'next' => fn() => $reader->next(),
        'expand' => fn() => $reader->expand(),
        'readOuterXml' => fn() => $reader->readOuterXml(),
        'moveToFirstAttribute' => fn() => $reader->moveToFirstAttribute(),
        'setParserProperty' => fn() => $reader->setParserProperty(XMLReader::SUBST_ENTITIES, true),
        'setSchema' => fn() => $reader->setSchema(null),
        'setRelaxNGSchema' => fn() => $reader->setRelaxNGSchema(null),
        'open' => fn() => $reader->open(__FILE__),
        'XML' => fn() => $reader->XML('<b/>'),
    ];
    foreach ($calls as $name => $call) {
        try {
            $call();
            echo "$name: no error\n";
        } catch (Error $e) {
            echo "$name: ", $e->getMessage(), "\n";
        }
    }
    var_dump($reader->depth, $reader->getParserProperty(XMLReader::SUBST_ENTITIES));
    /* only the frame of the running read keeps the object alive */
    $reader = null;
    fwrite($writer, '<root><a x="1"/></root>');
    fclose($writer);
});

$scheduler->loop();
var_dump($reader);
?>
--EXPECT--
close: Attempt to use XMLReader while it is reading
read: Attempt to use XMLReader while it is reading
next: Attempt to use XMLReader while it is reading
expand: Attempt to use XMLReader while it is reading
readOuterXml: Attempt to use XMLReader while it is reading
moveToFirstAttribute: Attempt to use XMLReader while it is reading
setParserProperty: Attempt to use XMLReader while it is reading
setSchema: Attempt to use XMLReader while it is reading
setRelaxNGSchema: Attempt to use XMLReader while it is reading
open: Attempt to use XMLReader while it is reading
XML: Attempt to use XMLReader while it is reading
int(0)
bool(false)
bool(true)
A: done
NULL
