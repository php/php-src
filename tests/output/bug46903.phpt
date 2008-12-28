--TEST--
Bug #46903 (ob_start(): Special $chunk_size value of 1 is not honoured in HEAD)
--FILE--
<?php
function flushCounter($input) {
	static $counter=0;
	return '[' . ++$counter . "] $input \n";
}

// This should set the buffer size to 4096
ob_start('flushCounter', 1);

// Get the buffer size: 
$bufferInfo = ob_get_status(true);
var_dump($bufferInfo[0]['chunk_size']);

// If the buffer size is >1, these two chars should
// come out as part of a single flush:
echo "1";
echo "2";
?>
--EXPECTF--
[1] int(4096)
12 