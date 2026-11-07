--TEST--
Test Csv\LazyLaxCollection::createFromFile(): the stream of a collection alive at shutdown is closed while userland can still run
--EXTENSIONS--
csv
--FILE--
<?php
class Wrapper {
    public $context;
    private static int $pos = 0;
    private static string $data = "a,b\r\nc,d\r\n";
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_read($count) {
        $chunk = substr(self::$data, self::$pos, $count);
        self::$pos += strlen($chunk);
        return $chunk;
    }
    public function stream_eof() { return self::$pos >= strlen(self::$data); }
    public function stream_close() { echo "stream_close() called\n"; }
    public function stream_stat() { return []; }
}
stream_wrapper_register('csvtest', Wrapper::class);

/* A cycle keeps the collection alive until shutdown */
class Holder { public $collection; public $self; }
$holder = new Holder;
$holder->self = $holder;
$holder->collection = Csv\LazyLaxCollection::createFromFile('csvtest://data');
foreach ($holder->collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
echo "end of script\n";
?>
--EXPECT--
["a","b"]
["c","d"]
end of script
stream_close() called
