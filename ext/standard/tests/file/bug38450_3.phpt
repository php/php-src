--TEST--
Bug #38450 (constructor is not called for classes used in userspace stream wrappers)
--FILE--
<?php

class VariableStream {
	var $position;
	var $varname;

	function __construct(array $var) {
		var_dump("constructor!");
	}

	function stream_open($path, $mode, $options, &$opened_path)
	{
		$url = parse_url($path);
		$this->varname = $url["host"];
		$this->position = 0;

		return true;
	}

	function stream_read($count)
	{
		$ret = substr($GLOBALS[$this->varname], $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	function stream_write($data)
	{
		$left = substr($GLOBALS[$this->varname], 0, $this->position);
		$right = substr($GLOBALS[$this->varname], $this->position + strlen($data));
		$GLOBALS[$this->varname] = $left . $data . $right;
		$this->position += strlen($data);
		return strlen($data);
	}

	function stream_tell()
	{
		return $this->position;
	}

	function stream_eof()
	{
		return $this->position >= strlen($GLOBALS[$this->varname]);
	}
	function stream_seek($offset, $whence)
	{
		switch ($whence) {
		case SEEK_SET:
			if ($offset < strlen($GLOBALS[$this->varname]) && $offset >= 0) {
				$this->position = $offset;
				return true;
			} else {
				return false;
			}
			break;

		case SEEK_CUR:
			if ($offset >= 0) {
				$this->position += $offset;
				return true;
			} else {
				return false;
			}
			break;

		case SEEK_END:
			if (strlen($GLOBALS[$this->varname]) + $offset >= 0) {
				$this->position = strlen($GLOBALS[$this->varname]) + $offset;
				return true;
			} else {
				return false;
			}
			break;

		default:
			return false;
		}
	}
}

stream_wrapper_register("var", "VariableStream")
	or die("Failed to register protocol");

$myvar = "";

$fp = fopen("var://myvar", "r+");

fwrite($fp, "line1\n");
fwrite($fp, "line2\n");
fwrite($fp, "line3\n");

rewind($fp);
while (!feof($fp)) {
	echo fgets($fp);
}
fclose($fp);
var_dump($myvar);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too few arguments to function VariableStream::__construct(), 0 passed and exactly 1 expected in %sbug38450_3.php:7
Stack trace:
#0 [internal function]: VariableStream->__construct()
#1 %s(%d): fopen('var://myvar', 'r+')
#2 {main}
  thrown in %s on line %d
