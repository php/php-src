--TEST--
GH-13071 (Copying large files using mmap-able source streams may exhaust available memory and fail)
--FILE--
<?php

class CustomStream {
    public $context;
    protected $file;
    protected $seekable;
    public static int $writes = 0;

    public function stream_open($path, $mode, $options, &$opened_path) {
        $path = $this->trim_path($path);
        $this->file = fopen($path, $mode);
        return true;
    }

    public function stream_close() {
        fclose($this->file);
        return true;
    }

    public function stream_write($data) {
        self::$writes++;
        return fwrite($this->file, $data);
    }

    public function url_stat($path, $flags) {
        return false;
    }

    private function trim_path(string $path): string {
        return substr($path, strlen("up://"));
    }
}

file_put_contents(__DIR__ . "/gh13071.tmp", str_repeat("a", 1024 * 1024 * 8));

stream_wrapper_register("up", CustomStream::class, STREAM_IS_URL);

$old_limit = ini_get("memory_limit");
ini_set("memory_limit", memory_get_usage(true) + 5 * 1024 * 1024);
copy(__DIR__ . "/gh13071.tmp", "up://" . __DIR__ . "/gh13071.out.tmp");
ini_set("memory_limit", $old_limit);

echo "Done ", CustomStream::$writes, " writes\n";

?>
--EXPECT--
Done 1024 writes
