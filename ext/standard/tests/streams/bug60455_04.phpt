--TEST--
Bug #60455: stream_get_line and 1-line with maxlen size followed by 0-length
read with EOL indication
--FILE--
<?php
class TestStream {
	private $s = 0;
	function stream_open($path, $mode, $options, &$opened_path) {
	        return true;
	}
	function stream_read($count) {
		if ($this->s++ == 0)
			return "a\n";
		
		return "";
	}
	function stream_eof() {
		return $this->s >= 2;
	}
	
}

stream_wrapper_register("test", "TestStream");

$f = fopen("test://", "r");
while (!feof($f)) {
    $line = stream_get_line($f, 2, "\n");
    var_dump($line);
}
--EXPECT--
string(1) "a"
bool(false)
