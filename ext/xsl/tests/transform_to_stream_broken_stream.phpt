--TEST--
XSLTProcessor::transformToStream() function - broken stream
--EXTENSIONS--
xsl
--FILE--
<?php
class MyStream {
    public $context;
    private bool $first = true;

    public function stream_write(string $data): int {
        if ($this->first) {
            $this->first = false;
            var_dump($data);
        }
        throw new Error("broken");
    }

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path) {
        return true;
    }

    public function stream_close(): void {
    }

    public function stream_eof(): bool {
        return !$this->first;
    }
}

stream_wrapper_register("foo", MyStream::class);

include("prepare.inc");
$proc->importStylesheet($xsl);

$stream = fopen('foo://', 'w');
stream_set_chunk_size($stream, 4);
$written = $proc->transformToStream($dom, $stream);
fclose($stream);

echo "\n";
var_dump($written);
?>
--EXPECTF--
string(4) "<?xm"

Fatal error: Uncaught Error: broken in %s:%d
Stack trace:
#0 [internal function]: MyStream->stream_write('<?xm')
#1 %s(%d): XSLTProcessor->transformToStream(Object(DOMDocument), Resource id #%d)
#2 {main}
  thrown in %s on line %d
