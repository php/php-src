--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - access/update file through hard link
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

/* Variation 4 : Create file and a hard link to the file
                 Access data of the file through the hard link
                 Update the file through hard link
                 Check size of file and hard link
*/
$file_path = __DIR__;

echo "*** Accessing and updating data of file through hard link ***\n";
// Creating file and inserting data into it
$filename = "$file_path/symlink__link_linkinfo_is_link_variation4.tmp";
// create temp file
$file = fopen($filename, "w");
// fill data into file
fwrite($file, str_repeat("text", 20) );
fclose($file);

echo "\n-- Access data of the file through the hard link --\n";
// create hard link to file
$linkname = "$file_path/symlink_link_linkinfo_is_link_link_variation4.tmp";
var_dump( link($filename, $linkname) );
$data_from_link = file_get_contents($linkname);  // data read from $filename
var_dump( $data_from_link );

echo "\n-- Check size of hard link and file --\n";
if( filesize($filename) == filesize($linkname) )
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

echo "\n-- Updating file with data through hard link --\n";
// append link with data
$fp = fopen($linkname, "a");  // open in append mode
fwrite($fp, "Hello World");
fclose($fp);

// now check temp file for data; it should append "Hello World"
$data_from_file = file_get_contents($filename);
var_dump( $data_from_file );

echo "\n-- Check size of hard link and file --\n";
if( filesize($filename) == filesize($linkname) )
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

echo "\n-- Updating file with data and check data through hard link --\n";
// write to temp file
$file = fopen($filename, "w");
fwrite($file, "Hello World");
fclose($file);

// now check link for data; it should echo "Hello World"
$data_from_link = file_get_contents($linkname);
var_dump( $data_from_link );

echo "\n-- Check size of hard link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );
if( filesize($filename) == filesize($linkname) )
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

// delete the link
unlink($linkname);
// delete the temporary file
unlink($filename);

echo "Done\n";
?>
--EXPECT--
*** Accessing and updating data of file through hard link ***

-- Access data of the file through the hard link --
bool(true)
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data through hard link --
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data and check data through hard link --
string(11) "Hello World"

-- Check size of hard link and file --
int(11)
int(11)

Size of file and hard link are same
Done
