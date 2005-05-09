<?php
if (!extension_loaded('mysqli')){
	@dl('mysqli.so');
	if (!extension_loaded('mysqli')) {
		die('skip mysqli extension not available');
	}
}
?>
