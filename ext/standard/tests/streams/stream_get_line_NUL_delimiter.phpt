--TEST--
Bug #60455: stream_get_line and \0 as a delimiter
--FILE--
<?php
class TestStream {
	private $s = 0;
	function stream_open($path, $mode, $options, &$opened_path) {
	        return true;
	}
	function stream_read($count) {
		if ($this->s++ == 0)
			return "a\0";

		return "";
	}
	function stream_eof() {
		return $this->s >= 2;
	}

}

stream_wrapper_register("test", "TestStream");

$f = fopen("test://", "r");
var_dump(stream_get_line($f, 100, "\0"));
--EXPECT--
string(1) "a"
