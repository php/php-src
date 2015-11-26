--TEST--
Verbose tidy_get_error_buffer()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php 
	$a = tidy_parse_string("<HTML><asd asdf></HTML>");
	echo tidy_get_error_buffer($a);
	
?>
--EXPECT--
line 1 column 1 - Warning: missing <!DOCTYPE> declaration
line 1 column 7 - Error: <asd> is not recognized!
line 1 column 7 - Warning: discarding unexpected <asd>
line 1 column 17 - Warning: discarding unexpected </html>
line 1 column 7 - Warning: inserting missing 'title' element