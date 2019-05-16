--TEST--
Stream wrappers in include_path
--FILE--
<?php
$data1 = $data2 = $data3 = $data4 = $data5 = $data6 = <<<'EOD'
<?php echo __FILE__ . "\n";?>

EOD;
/*<?*/

class mystream
{
	public $path;
	public $mode;
	public $options;

	public $position;
	public $varname;

	function url_stat($path, $flags) {
		return array();
	}

	function stream_stat() {
		return array();
	}

	function stream_open($path, $mode, $options, &$opened_path)
	{
		$this->path = $path;
		$this->mode = $mode;
		$this->options = $options;

		$split = parse_url($path);
		if ($split["host"] !== "GLOBALS" ||
		    empty($split["path"]) ||
		    empty($GLOBALS[substr($split["path"],1)])) {
		    return false;
		}
		$this->varname = substr($split["path"],1);

		if (strchr($mode, 'a'))
			$this->position = strlen($GLOBALS[$this->varname]);
		else
			$this->position = 0;

		return true;
	}

	function stream_read($count)
	{
		$ret = substr($GLOBALS[$this->varname], $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
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
		switch($whence) {
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

if (!stream_wrapper_register("test", "mystream")) {
	die("test wrapper registration failed");
}

echo file_get_contents("test://GLOBALS/data1");
include("test://GLOBALS/data1");
include_once("test://GLOBALS/data2");
include_once("test://GLOBALS/data2");
$include_path = get_include_path();
set_include_path($include_path . PATH_SEPARATOR . "test://GLOBALS");
echo file_get_contents("data3", true);
include("data3");
include_once("data4");
include_once("data4");
set_include_path("test://GLOBALS"  . PATH_SEPARATOR .  $include_path);
echo file_get_contents("data5", true);
include("data5");
include_once("data6");
include_once("data6");
?>
--EXPECT--
<?php echo __FILE__ . "\n";?>
test://GLOBALS/data1
test://GLOBALS/data2
<?php echo __FILE__ . "\n";?>
test://GLOBALS/data3
test://GLOBALS/data4
<?php echo __FILE__ . "\n";?>
test://GLOBALS/data5
test://GLOBALS/data6
