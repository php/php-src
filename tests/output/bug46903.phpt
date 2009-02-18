--TEST--
Bug #46903: ob_start(): In HEAD, $chunk_size value of 1 should not have any special behaviour (Updated as per: http://marc.info/?l=php-internals&m=123476465621346&w=2)
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
[1] int(1)
 
[2] 1 
[3] 2 
[4]