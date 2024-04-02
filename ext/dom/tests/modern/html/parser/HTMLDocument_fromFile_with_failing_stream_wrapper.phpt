--TEST--
DOM\HTMLDocument::createFromFile() with failing stream wrapper
--EXTENSIONS--
dom
--FILE--
<?php

class FailingWrapper {
    public $context;
    public $fail = false;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_read($count) {
        if ($this->fail) {
            throw new Error("fail");
        }
        $this->fail = true;
        return str_repeat("X", $count);
    }

    public function stream_eof() {
        return false;
    }

    public function stream_close() {
        return true;
    }
}

stream_wrapper_register("fail", FailingWrapper::class, 0);

DOM\HTMLDocument::createFromFile("fail://x");

?>
--EXPECTF--
Fatal error: Uncaught Error: fail in %s:%d
Stack trace:
#0 [internal function]: FailingWrapper->stream_read(8192)
#1 %s(%d): DOM\HTMLDocument::createFromFile('fail://x')
#2 {main}
  thrown in %s on line %d
