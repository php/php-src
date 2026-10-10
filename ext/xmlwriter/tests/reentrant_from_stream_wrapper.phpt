--TEST--
XMLWriter: the writer refuses use from a stream wrapper it is writing to
--EXTENSIONS--
xmlwriter
--FILE--
<?php
class Wrapper {
    public static $log = [];
    public static $data = '';
    public $context;
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_write($data) {
        global $writer;
        foreach (['text', 'flush', 'openMemory'] as $method) {
            try {
                $method === 'text' ? $writer->text('y') : $writer->$method();
                self::$log[] = "$method: no error";
            } catch (Error $e) {
                self::$log[] = "$method: " . $e->getMessage();
            }
        }
        self::$data .= $data;
        return strlen($data);
    }
    public function stream_close() {}
}
stream_wrapper_register('test', 'Wrapper');

$writer = new XMLWriter();
var_dump($writer->openUri('test://doc'));
$writer->startElement('r');
$writer->text(str_repeat('x', 10000));
$writer->endElement();
var_dump($writer->flush());
var_dump(Wrapper::$data === '<r>' . str_repeat('x', 10000) . '</r>');
echo implode("\n", array_unique(Wrapper::$log)), "\n";
?>
--EXPECTF--
bool(true)
int(%d)
bool(true)
text: Attempt to use XMLWriter while it is writing
flush: Attempt to use XMLWriter while it is writing
openMemory: Attempt to use XMLWriter while it is writing
