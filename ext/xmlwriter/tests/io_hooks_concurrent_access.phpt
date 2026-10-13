--TEST--
XMLWriter under io_hooks: the writer refuses other use while a write waits for the output
--EXTENSIONS--
xmlwriter
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

[$out, $in] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$writer = XMLWriter::toStream($out);
$len = 1 << 20;

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use (&$writer, $len) {
    $w = $writer;
    $w->startElement('r');
    /* more than the socket buffer takes */
    var_dump($w->text(str_repeat('x', $len)));
    var_dump($w->endElement(), $w->flush());
});

$scheduler->spawn(function () use (&$writer, $in, $len) {
    $calls = [
        'text' => fn() => $writer->text('y'),
        'endElement' => fn() => $writer->endElement(),
        'flush' => fn() => $writer->flush(),
        'outputMemory' => fn() => $writer->outputMemory(),
        'xmlwriter_text' => fn() => xmlwriter_text($writer, 'y'),
        'openMemory' => fn() => $writer->openMemory(),
        'openUri' => fn() => $writer->openUri('php://memory'),
    ];
    foreach ($calls as $name => $call) {
        try {
            $call();
            echo "$name: no error\n";
        } catch (Error $e) {
            echo "$name: ", $e->getMessage(), "\n";
        }
    }
    $writer = null;
    $data = '';
    while (strlen($data) < $len + 7 && !feof($in)) {
        $data .= fread($in, 65536);
    }
    var_dump($data === '<r>' . str_repeat('x', $len) . '</r>');
});

$scheduler->loop();
?>
--EXPECT--
text: Attempt to use XMLWriter while it is writing
endElement: Attempt to use XMLWriter while it is writing
flush: Attempt to use XMLWriter while it is writing
outputMemory: Attempt to use XMLWriter while it is writing
xmlwriter_text: Attempt to use XMLWriter while it is writing
openMemory: Attempt to use XMLWriter while it is writing
openUri: Attempt to use XMLWriter while it is writing
bool(true)
bool(true)
int(4)
bool(true)
