--TEST--
GH-25691 (Stack-use-after-return in ini_parse() with nested parse_ini_*() from a stream wrapper)
--CREDITS--
xuziqiang98
--FILE--
<?php
class W {
    public $context;
    private $done = false;

    public function stream_open($path, $mode, $options, &$opened_path) {
        var_dump(parse_ini_string("[sec]\nb=2", true));
        return true;
    }

    public function stream_read($count) {
        if ($this->done) {
            return "";
        }
        $this->done = true;
        return "a=1\n";
    }

    public function stream_eof() {
        return true;
    }

    public function stream_stat() {
        return [];
    }

    public function stream_set_option($option, $arg1, $arg2) {
        return false;
    }
}

stream_wrapper_register('ww', W::class);
var_dump(parse_ini_file('ww://x', true));
?>
--EXPECT--
array(1) {
  ["sec"]=>
  array(1) {
    ["b"]=>
    string(1) "2"
  }
}
array(1) {
  ["a"]=>
  string(1) "1"
}
