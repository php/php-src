--TEST--
Test fileinode() function: Variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no link()/symlink() on Windows');
}
--FILE--
<?php
/* 
Prototype: int fileinode ( string $filename );
Description: Returns the inode number of the file, or FALSE in case of an error.
*/

include "file.inc";

echo "*** Testing fileinode() with files, links and directories ***\n";

echo "-- Testing with files --\n";
create_files( dirname(__FILE__), 2);

print( fileinode( dirname(__FILE__)."/file1.tmp") )."\n";
print( fileinode( dirname(__FILE__)."/file2.tmp") )."\n";
clearstatcache();

echo "-- Testing with links: hard link --\n";
link( dirname(__FILE__)."/file1.tmp", dirname(__FILE__)."/link1.tmp");  // Creating an hard link
print( fileinode( dirname(__FILE__)."/file1.tmp") )."\n";
clearstatcache();
print( fileinode( dirname(__FILE__)."/link1.tmp") )."\n";
clearstatcache();

echo "-- Testing with links: soft link --\n";
symlink( dirname(__FILE__)."/file2.tmp", dirname(__FILE__)."/link2.tmp");  // Creating a soft link
print( fileinode( dirname(__FILE__)."/file2.tmp") )."\n";
clearstatcache();
print( fileinode( dirname(__FILE__)."/link2.tmp") )."\n";

delete_files( dirname(__FILE__), 2, "link");

echo "-- Testing after copying a file --\n";
copy( dirname(__FILE__)."/file1.tmp", dirname(__FILE__)."/file1_new.tmp");
print( fileinode( dirname(__FILE__)."/file1.tmp") )."\n";
clearstatcache();
print( fileinode( dirname(__FILE__)."/file1_new.tmp") )."\n";

unlink( dirname(__FILE__)."/file1_new.tmp");

delete_files( dirname(__FILE__), 2);


echo "-- Testing after renaming the file --\n";
$file_path = dirname(__FILE__);
fopen("$file_path/old.txt", "w");
print( fileinode("$file_path/old.txt") )."\n";
clearstatcache();

rename("$file_path/old.txt", "$file_path/new.txt");
print( fileinode("$file_path/new.txt") )."\n";

unlink("$file_path/new.txt");

echo "-- Testing with directories --\n";
mkdir("$file_path/dir");
print( fileinode("$file_path/dir") )."\n";
clearstatcache();

mkdir("$file_path/dir/subdir");
print( fileinode("$file_path/dir/subdir") );

rmdir("$file_path/dir/subdir");
rmdir("$file_path/dir");

echo "\n*** Done ***";

--EXPECTF--
*** Testing fileinode() with files, links and directories ***
-- Testing with files --
%d
%d
-- Testing with links: hard link --
%d
%d
-- Testing with links: soft link --
%d
%d
-- Testing after copying a file --
%d
%d
-- Testing after renaming the file --
%d
%d
-- Testing with directories --
%d
%d
*** Done ***
