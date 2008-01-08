<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
class corrupter extends tarmaker {
function close()
{
	parent::close();
	$fp = fopen($this->path, 'r+b');
	fseek($fp, 20);
	fwrite($fp, 'oopsie');
	fclose($fp);
}
}