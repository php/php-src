--TEST--
Test copy() function: usage variations - non existing src/dest
--FILE--
<?php
/* Test copy(): Trying to create a copy of non-existing source in an existing destination
     and an existing source in non-existing destination */

$file_path = __DIR__;

echo "*** Test copy() function: Trying to create a copy of non-existing source in existing destination ***";
$file = $file_path."/copy_variation14.tmp";
$file_handle =  fopen($file, "w");
fwrite($file_handle, str_repeat("Hello2world...\n", 100));
fclose($file_handle);

var_dump( copy($file_path."/nosuchfile.tmp", $file_path."/copy_nosuchfile.tmp") );  //With non-existing source
var_dump( file_exists($file_path."/copy_nosuchfile.tmp") );

echo "\n*** Test copy() function: Trying to create copy of an existing source in non-existing destination ***";
var_dump( copy($file, $file_path."/nodir/copy_nosuchfile.tmp") );  //With non-existing dir path
var_dump( file_exists($file_path."/nodir/copy_nosuchfile.tmp") );
var_dump( filesize($file) );  //size of the source

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/copy_variation14.tmp");
?>
--EXPECTF--
*** Test copy() function: Trying to create a copy of non-existing source in existing destination ***
Warning: copy(%s): %s
bool(false)
bool(false)

*** Test copy() function: Trying to create copy of an existing source in non-existing destination ***
Warning: copy(%s): %s
bool(false)
bool(false)
int(1500)
*** Done ***
