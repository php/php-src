--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - access/update file through softlink
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
/* Prototype: bool symlink ( string $target, string $link );
   Description: creates a symbolic link to the existing target with the specified name link

   Prototype: bool is_link ( string $filename );
   Description: Tells whether the given file is a symbolic link.

   Prototype: bool link ( string $target, string $link );
   Description: Create a hard link

   Prototype: int linkinfo ( string $path );
   Description: Gets information about a link
*/

/* Variation 3 : Create file and a soft link to the file
                 Access data of the file through the soft link
                 Update the file through soft link
                 Check size of file and soft link link
*/

$file_path = dirname(__FILE__);
echo "*** Accessing and updating data of file through soft link ***\n";
// Creating file and inserting data into it
$filename = "$file_path/symlink_link_linkinfo_is_link_variation3.tmp";

// create temp file
$file = fopen($filename, "w");

// create soft link to file
$linkname = "$file_path/symlink_link_linkinfo_is_link_link_variation3.tmp";
var_dump( symlink($filename, $linkname) );
// storing size of symlink in a local variable
$link_stat = lstat($linkname);  // lstat of link
$link_size = $link_stat[7];  // size of soft link

// fill data into file
fwrite($file, str_repeat("text", 20) );
fclose($file);

echo "\n-- Access data of the file through the soft link --\n";
$data_from_link = file_get_contents($linkname);  // data read from $filename
var_dump( $data_from_link );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

echo "\n-- Updating file with data through soft link --\n";
// append link with data
$fp = fopen($linkname, "a");  // open in append mode
fwrite($fp, "Hello World");
fclose($fp);

// now check temp file for data; it should append "Hello World"
$data_from_file = file_get_contents($filename);
var_dump( $data_from_file );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

echo "\n-- Updating file with data and check data through soft link --\n";
// write to temp file
$file = fopen($filename, "w");
fwrite($file, "Hello World");
fclose($file);

// now check link for data; it should echo "Hello World"
$data_from_link = file_get_contents($linkname);
var_dump( $data_from_link );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

// delete the link
unlink($linkname);
// delete the temporary file
unlink($filename);

echo "Done\n";
?>
--EXPECT--
*** Accessing and updating data of file through soft link ***
bool(true)

-- Access data of the file through the soft link --
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of soft link and file --
int(80)
int(80)

Soft link size remains same 

-- Updating file with data through soft link --
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of soft link and file --
int(91)
int(91)

Soft link size remains same 

-- Updating file with data and check data through soft link --
string(11) "Hello World"

-- Check size of soft link and file --
int(11)
int(11)

Soft link size remains same 
Done
