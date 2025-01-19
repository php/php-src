--TEST--
simplexml_load_stream() - from broken stream
--EXTENSIONS--
simplexml
--FILE--
<?php

class MyStream {
    public $context;
    private bool $first = true;

    public function stream_read(int $count): string|false {
        var_dump($count);
        if ($this->first) {
            $this->first = false;
            return "<root><child/>";
        }
        return false;
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

$tmp = fopen("foo://", "r");
$sxe = simplexml_load_stream($tmp);
fclose($tmp);

var_dump($sxe);

?>
--EXPECTF--
int(8192)
int(8192)
%A
Warning: simplexml_load_stream(): Entity: line 1: parser error : %s

Warning: simplexml_load_stream(): <root><child/> in %s on line %d

Warning: simplexml_load_stream():%s^ in %s on line %d
bool(false)
