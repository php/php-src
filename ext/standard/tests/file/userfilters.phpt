--TEST--
User-space filters
--FILE--
<?php
# vim600:syn=php:

class UpperCaseFilter extends php_user_filter {
	function oncreate()
	{
		echo "oncreate:\n";
		var_dump($this->filtername);
		var_dump($this->params);
	}

	function flush($closing)
	{
		echo "flush:\n";
	}

	function onclose()
	{
		echo "onclose:\n";
	}

	function write($data)
	{
		echo "write:\n";
		$x = parent::write($data);
		return $x;
	}

	function read($bytes)
	{
		echo "read:\n";
		$x = parent::read($bytes);
		return strtoupper($x);
	}
};

var_dump(stream_register_filter("string.uppercase", "UpperCaseFilter"));
$fp = tmpfile();

fwrite($fp, "hello there");
rewind($fp);

var_dump(stream_filter_prepend($fp, "string.uppercase"));
var_dump(fgets($fp));
fclose($fp);
?>
--EXPECT--
bool(true)
oncreate:
string(16) "string.uppercase"
NULL
bool(true)
read:
read:
string(11) "HELLO THERE"
flush:
onclose:
