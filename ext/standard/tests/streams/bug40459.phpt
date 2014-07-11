--TEST--
bug 40459 - Test whether the constructor of the user-space stream wrapper is called when stream functions are called
--FILE--
<?php
// Test whether the constructor of the user-space stream wrapper is called when stream functions are called
class testwrapper {
	private $constructorCalled = false;
	function __construct() {
		$this->constructorCalled = true;
	}

	function stream_open($path, $mode, $options, &$opened_path)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
	  	return true;
	}

	function url_stat($url, $flags)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
		return array();
	}

	function unlink($url)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
	}

	function rename($from, $to)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
	}

	function mkdir($dir, $mode, $options)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
	}

	function rmdir($dir, $options)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
	}

	function dir_opendir($url, $options)
	{
		echo $this->constructorCalled ? 'yes' : 'no';
		return TRUE;
	}
	function stream_metadata() 
	{
		echo $this->constructorCalled ? 'yes' : 'no';
		return TRUE;
	}
}

stream_wrapper_register('test', 'testwrapper', STREAM_IS_URL);

echo 'stream_open: ';
fopen('test://test', 'r');
echo "\n";

echo 'url_stat: ';
stat('test://test');
echo "\n";

echo 'dir_opendir: ';
opendir('test://test');
echo "\n";

echo 'rmdir: ';
rmdir('test://test');
echo "\n";

echo 'mkdir: ';
mkdir('test://test');
echo "\n";

echo 'rename: ';
rename('test://test', 'test://test2');
echo "\n";

echo 'unlink: ';
unlink('test://test');
echo "\n";

echo 'touch: ';
touch('test://test', time());
echo "\n";



?>
==DONE==
--EXPECT--
stream_open: yes
url_stat: yes
dir_opendir: yes
rmdir: yes
mkdir: yes
rename: yes
unlink: yes
touch: yes
==DONE==
