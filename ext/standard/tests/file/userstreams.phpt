--TEST--
User-space streams
--FILE--
<?php
# vim600:syn=php:

class uselessstream {
}

class mystream {

	function mystream()
	{
		echo "MYSTREAM: constructor called!\n";
	}

	var $path;
	var $mode;
	var $options;

	function stream_open($path, $mode, $options, &$opened_path)
	{
		$this->path = $path;
		$this->mode = $mode;
		$this->options = $options;
		return true;
	}

}

if (@file_register_wrapper("bogus", "class_not_exist"))
	die("Registered a non-existant class!!!???");

if (!file_register_wrapper("test", "mystream"))
	die("test wrapper registration failed");
if (!file_register_wrapper("bogon", "uselessstream"))
	die("bogon wrapper registration failed");

echo "Registered\n";

$b = @fopen("bogon://url", "rb");
if (is_resource($b))
	die("Opened a bogon??");

$fp = fopen("test://url", "rb");
if (!is_resource($fp))
	die("Failed to open resource");

?>
--EXPECT--
Registered
