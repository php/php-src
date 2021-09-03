--TEST--
Test md5_file() function with ASCII output and raw binary output
--FILE--
<?php

/* Creating an empty file */
if (($handle = fopen( "EmptyFileMD5.txt", "w+")) == FALSE)
return false;

/* Creating a data file */
if (($handle2 = fopen( "DataFileMD5.txt", "w+")) == FALSE)
return false;

/* Writing into file */
$filename = "DataFileMD5.txt";
$content = "Add this to the file\n";
if (is_writable($filename)) {
  if (fwrite($handle2, $content) === FALSE) {
    echo "Cannot write to file ($filename)";
    exit;
  }
}

// close the files
fclose($handle);
fclose($handle2);

/* Testing error conditions */
echo "\n*** Testing for error conditions ***\n";

/* No filename */
try {
    var_dump( md5_file("") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
/* invalid filename */
var_dump( md5_file("aZrq16u") );

/* Scalar value as filename  */
var_dump( md5_file(12) );

/* Hexadecimal Output for Empty file as input */
echo "\n*** Hexadecimal Output for Empty file as Argument ***\n";
var_dump( md5_file("EmptyFileMD5.txt") );

/* Raw Binary Output for Empty file as input */
echo "\n*** Raw Binary Output for Empty file as Argument ***\n";
var_dump( md5_file("EmptyFileMD5.txt", true) );

/* Normal operation with hexadecimal output */
echo "\n*** Hexadecimal Output for a valid file with some contents ***\n";
var_dump( md5_file("DataFileMD5.txt") );

/* Normal operation with raw binary output */
echo "\n*** Raw Binary Output for a valid file with some contents ***\n";
var_dump ( md5_file("DataFileMD5.txt", true) );

// remove temp files
unlink("DataFileMD5.txt");
unlink("EmptyFileMD5.txt");

echo "\nDone";
?>
--EXPECTF--
*** Testing for error conditions ***
Path cannot be empty

Warning: md5_file(aZrq16u): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: md5_file(12): Failed to open stream: No such file or directory in %s on line %d
bool(false)

*** Hexadecimal Output for Empty file as Argument ***
string(32) "d41d8cd98f00b204e9800998ecf8427e"

*** Raw Binary Output for Empty file as Argument ***
string(16) "‘åŸè%0≤ÈÄ	òÏ¯B~"

*** Hexadecimal Output for a valid file with some contents ***
string(32) "7f28ec647825e2a70bf67778472cd4a2"

*** Raw Binary Output for a valid file with some contents ***
string(16) "(Ïdx%‚ßˆwxG,‘¢"

Done
