--TEST--
fopencookie detected and working (or cast mechanism works)
--FILE--
<?php
# vim600:syn=php:

/* This test verifies that the casting mechanism is working correctly.
 * On systems with fopencookie, a FILE* is created around the user
 * stream and that is passed back to the ZE to include.
 * On systems without fopencookie, the stream is fed into a temporary
 * file, and that temporary file is passed back to the ZE.
 * The important thing here is really fopencookie; the glibc people
 * changed the binary interface, so if haven't detected it correctly,
 * you can expect this test to segfault.
 */

class userstream {
	var $position = 0;
	var $data = "If you can read this, it worked";
	
	function stream_open($path, $mode, $options, &$opened_path)
	{
		return true;
	}
	
	function stream_read($count)
	{
		$ret = substr($this->data, $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	function stream_tell()
	{
		return $this->position;
	}

	function stream_eof()
	{
		return $this->position >= strlen($this->data);
	}
}

stream_register_wrapper("cookietest", "userstream");

include("cookietest://foo");

?>
--EXPECT--
If you can read this, it worked
