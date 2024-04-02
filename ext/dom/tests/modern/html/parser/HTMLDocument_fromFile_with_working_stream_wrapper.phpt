--TEST--
DOM\HTMLDocument::createFromFile() with working stream wrapper
--EXTENSIONS--
dom
--FILE--
<?php

class EchoUriWrapper {
    private int $position = 0;
    private string $data;
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        $this->data = substr($path, 6);
        return true;
    }

    public function stream_read($count) {
        $ret = substr($this->data, $this->position, $count);
        $this->position += $count;
        return $ret;
    }

    public function stream_eof() {
        return $this->position >= strlen($this->data);
    }

    public function stream_close() {
        return true;
    }
}

stream_wrapper_register("euw", EchoUriWrapper::class, 0);

echo "--- Stream wrapper case ---\n";

$dom = DOM\HTMLDocument::createFromFile("euw://<p>hello</p>");
echo $dom->saveHTML(), "\n";

echo "--- Stream wrapper in two chunks case ---\n";

libxml_use_internal_errors(true);
// To properly test this, keep the 4096 in sync with document.c's input stream buffer size.
$dom = DOM\HTMLDocument::createFromFile("euw://<!doctype html><html>" . str_repeat("\n", 4096-22) . "<></html>");
echo $dom->saveHTML(), "\n";

foreach (libxml_get_errors() as $error) {
    var_dump($error->line, $error->column);
}

?>
--EXPECTF--
--- Stream wrapper case ---

Warning: DOM\HTMLDocument::createFromFile(): tree error unexpected-token-in-initial-mode in euw://<p>hello</p>, line: 1, column: 2 in %s on line %d
<html><head></head><body><p>hello</p></body></html>
--- Stream wrapper in two chunks case ---
<!DOCTYPE html><html><head></head><body>&lt;&gt;</body></html>
int(4075)
int(2)
