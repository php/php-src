--TEST--
Bug #27183 (userland stream wrapper segfaults on stream_write)
--FILE--
<?php
stream_wrapper_register("one", "StreamOne");
$fd = fopen("one://tmp/bla", "w");
fwrite($fd, "hmm");
fclose($fd);

class StreamOne {
	function stream_open ($path, $mode, $options, &$opened_path)
	{
        	$this->b = 'mmh';
		return true;
	}
        function stream_write($data)
        {
        	$this->a = $data;
	}
	function stream_close()
	{
        	debug_zval_dump($this->a);
                debug_zval_dump($this->b);
		return true;
	}
}
?>
--EXPECT--
string(3) "hmm" refcount(2)
string(3) "mmh" refcount(2)
