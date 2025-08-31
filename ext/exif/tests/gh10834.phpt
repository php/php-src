--TEST--
GH-10834 (exif_read_data() cannot read smaller stream wrapper chunk sizes)
--EXTENSIONS--
exif
--FILE--
<?php
class VariableStream {
	private $data;
	private $position;
	public $context;

	function stream_close() {
		return true;
	}

	function stream_eof() {
		return $this->position >= strlen($this->data);
	}

	function stream_open($path, $mode, $options, &$opened_path) {
		$this->position = 0;
		$this->data = file_get_contents(__DIR__.'/bug50845.jpg');
		return true;
	}

	function stream_seek($offset, $whence) {
		switch ($whence) {
			case SEEK_SET:
				if ($offset < strlen($this->data) && $offset >= 0) {
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
				if (strlen($this->data) + $offset >= 0) {
					$this->position = strlen($this->data) + $offset;
					return true;
				} else {
					return false;
				}
				break;
			default:
				return false;
		}
	}

	function stream_read($count) {
		$ret = substr($this->data, $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	function stream_tell() {
		return $this->position;
	}
}

stream_wrapper_register('var', 'VariableStream');

$fp = fopen('var://myvar', 'rb');

stream_set_chunk_size($fp, 10);
$headers = exif_read_data($fp);
var_dump(is_array($headers));

fclose($fp);
?>
--EXPECT--
bool(true)
