--TEST--
Dom\HTMLDocument::createFromStream() - broken stream
--EXTENSIONS--
dom
--FILE--
<?php

class MyStream {
    public $context;
    private bool $first = true;

    public function stream_read(int $count): string|false {
        if ($this->first) {
            $this->first = false;
            return "<!DOCTYPE html><html><p>Hello";
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

$tmp = fopen("foo://", "r+");
try {
    $dom = Dom\HTMLDocument::createFromStream($tmp);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
fclose($tmp);

?>
--EXPECT--
broken
