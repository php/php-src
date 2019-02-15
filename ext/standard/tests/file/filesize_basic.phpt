--TEST--
Test filesize() function: basic functionaity
--FILE--
<?php
/*
 * Prototype: int filesize ( string $filename );
 * Description: Returns the size of the file in bytes, or FALSE
 *              (and generates an error of level E_WARNING) in case of an error.
 */


echo "*** Testing size of files and directories with filesize() ***\n";

$file_path = dirname(__FILE__);

var_dump( filesize(__FILE__) );
var_dump( filesize(".") );

/* Empty file */
$file_name = $file_path."/filesize_basic.tmp";
$file_handle = fopen($file_name, "w");
fclose($file_handle);
var_dump( filesize($file_name) );

echo "*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$file_name = $file_path."/filesize_basic.tmp";
unlink($file_name);
?>
--EXPECTF--
*** Testing size of files and directories with filesize() ***
int(%d)
int(%d)
int(0)
*** Done ***
