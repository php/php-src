--TEST--
XMLReader: the reader refuses use from a stream wrapper it is reading from
--EXTENSIONS--
xmlreader
--FILE--
<?php
class Wrapper {
    public static $log = [];
    public $context;
    private $data = '<root><a/><b/></root>';
    private $pos = 0;
    public function url_stat($path, $flags) { return []; }
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_read($count) {
        global $reader;
        foreach (['close', 'read', 'next', 'expand', 'moveToFirstAttribute', 'XML'] as $method) {
            try {
                $method === 'XML' ? $reader->XML('<x/>') : $reader->$method();
                self::$log[] = "$method: no error";
            } catch (Error $e) {
                self::$log[] = "$method: " . $e->getMessage();
            }
        }
        $chunk = substr($this->data, $this->pos, 4);
        $this->pos += strlen($chunk);
        return $chunk;
    }
    public function stream_eof() { return $this->pos >= strlen($this->data); }
    public function stream_stat() { return []; }
    public function stream_close() {}
}
stream_wrapper_register('test', 'Wrapper');

$reader = new XMLReader();
var_dump($reader->open('test://doc'));
while ($reader->read()) {
    echo $reader->name, "\n";
}
echo implode("\n", array_unique(Wrapper::$log)), "\n";
?>
--EXPECT--
bool(true)
root
a
b
root
close: Attempt to use XMLReader while it is reading
read: Attempt to use XMLReader while it is reading
next: Attempt to use XMLReader while it is reading
expand: Attempt to use XMLReader while it is reading
moveToFirstAttribute: Attempt to use XMLReader while it is reading
XML: Attempt to use XMLReader while it is reading
