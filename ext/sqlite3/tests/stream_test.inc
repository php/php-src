<?php

class SQLite3_Test_Stream
{
	private $position;
	public static $string_length = 10;
	public static $string = "abcdefg\0hi";

	public function stream_open($path, $mode, $options, &$opened_path)
	{
		$this->position = 0;
		return true;
	}

	public function stream_read($count)
	{
		$ret = substr(self::$string, $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	public function stream_write($data)
	{
		return 0;
	}

	public function stream_stat()
	{
		return array('size' => self::$string_length);
	}

	public function stream_tell()
	{
		return $this->position;
	}

	public function stream_eof()
	{
		return ($this->position >= self::$string_length);
	}
}

stream_wrapper_register('sqliteBlobTest', "SQLite3_Test_Stream") or die("Unable to register sqliteBlobTest stream");

?>