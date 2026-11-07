--TEST--
Test Csv\LazyLaxCollection::createFromFile() and Csv\collection_to_file(): the default stream context is used
--EXTENSIONS--
csv
--FILE--
<?php
class Wrapper {
    public $context;
    private static string $data = '';
    private int $pos = 0;
    public function stream_open($path, $mode, $options, &$opened_path) {
        echo $mode, ': ', json_encode(stream_context_get_options($this->context)['csvtest'] ?? null), \PHP_EOL;
        return true;
    }
    public function stream_read($count) {
        $chunk = substr(self::$data, $this->pos, $count);
        $this->pos += strlen($chunk);
        return $chunk;
    }
    public function stream_write($data) { self::$data .= $data; return strlen($data); }
    public function stream_flush() { return true; }
    public function stream_eof() { return $this->pos >= strlen(self::$data); }
    public function stream_stat() { return []; }
}
stream_wrapper_register('csvtest', Wrapper::class);
stream_context_set_default(['csvtest' => ['option' => 'value']]);

Csv\collection_to_file('csvtest://data', [['a', 'b']]);
foreach (Csv\LazyLaxCollection::createFromFile('csvtest://data') as $row) {
    echo json_encode($row), \PHP_EOL;
}
?>
--EXPECT--
wb: {"option":"value"}
rb: {"option":"value"}
["a","b"]
