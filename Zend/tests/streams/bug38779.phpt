--TEST--
Bug #38779 (engine crashes when require()'ing file with syntax error through userspace stream wrapper)
--FILE--
<?php

class Loader {
    public $context;
    private $position;
    private $data;
    public function stream_open($path, $mode, $options, &$opened_path)  {
        $this->data = '<' . "?php \n\"\";ll l\n ?" . '>';
        $this->position = 0;
        return true;
    }
    function stream_read($count) {
        $ret = substr($this->data, $this->position, $count);
        $this->position += strlen($ret);
        return $ret;
    }
    function stream_eof() {
        return $this->position >= strlen($this->data);
    }
    function stream_stat() {
        return array('size' => strlen($this->data));
    }
    function stream_set_option($option, $arg1, $arg2) {
        return false;
    }
}
stream_wrapper_register('Loader', 'Loader');
require 'Loader://qqq.php';

echo "Done\n";
?>
--EXPECTF--
Parse error: %s error%sin Loader://qqq.php on line %d
