--TEST--
User streams and include()
--INI--
allow_url_fopen=1
allow_url_include=0
--FILE--
<?php
class test {
    private $data = '<?php echo "Hello World\n";?>';
	private $pos;

	function stream_open($path, $mode, $options, &$opened_path)
	{
		if (strchr($mode, 'a'))
			$this->pos = strlen($this->data);
		else
			$this->po = 0;

		return true;
	}

	function stream_read($count)
	{
		$ret = substr($this->data, $this->pos, $count);
		$this->pos += strlen($ret);
		return $ret;
	}

	function stream_tell()
	{
		return $this->pos;
	}

	function stream_eof()
	{
		return $this->pos >= strlen($this->data);
	}

	function stream_seek($offset, $whence)
	{
		switch($whence) {
			case SEEK_SET:
				if ($offset < $this->data && $offset >= 0) {
					$this->pos = $offset;
					return true;
				} else {
					return false;
				}
				break;
			case SEEK_CUR:
				if ($offset >= 0) {
					$this->pos += $offset;
					return true;
				} else {
					return false;
				}
				break;
			case SEEK_END:
				if (strlen($this->data) + $offset >= 0) {
					$this->pos = strlen($this->data) + $offset;
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

stream_register_wrapper("test1", "test", STREAM_IS_URL);
stream_register_wrapper("test2", "test");
echo @file_get_contents("test1://hello"),"\n";
@include "test1://hello";
echo @file_get_contents("test2://hello"),"\n";
@include "test2://hello";
--EXPECT--
<?php echo "Hello World\n";?>
<?php echo "Hello World\n";?>
Hello World
