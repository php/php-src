--TEST--
Bug #38779 (engine crashes when require()'ing file with syntax error through userspace stream wrapper)
--FILE--
<?php

class Loader {
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
	function stream_flush() {
		var_dump("flush!");
	}
	function stream_close() {
		@unlink(dirname(__FILE__)."/bug38779.txt");
		var_dump("close!");
	}
}
stream_wrapper_register('Loader', 'Loader');
$fp = fopen ('Loader://qqq.php', 'r');

$filename = dirname(__FILE__)."/bug38779.txt";
$fp1 = fopen($filename, "w");
fwrite($fp1, "<"."?php blah blah?".">");
fclose($fp1);

include $filename;

echo "Done\n";
?>
--CLEAN--
<?php

$filename = dirname(__FILE__)."/bug38779.txt";
if (file_exists($filename)) {
	@unlink(dirname(__FILE__)."/bug38779.txt");
}
?>
--EXPECTF--	
Parse error: %s error%sin %s on line %d
string(6) "close!"
