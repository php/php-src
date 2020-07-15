--TEST--
Test fileinode() function: Variations
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    require_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
/*
Prototype: int fileinode ( string $filename );
Description: Returns the inode number of the file, or FALSE in case of an error.
*/

echo "*** Testing fileinode() with files, links and directories ***\n";
$file_path = __DIR__;
$file1 = $file_path."/fileinode1_variation.tmp";
$file2 = $file_path."/fileinode2_variation.tmp";
$link1 = $file_path."/fileinode1_variation_link.tmp";
$link2 = $file_path."/fileinode2_variation_link.tmp";


echo "-- Testing with files --\n";
//creating the files
fclose( fopen( $file1, "w" ) );
fclose( fopen( $file2, "w" ) );

print( fileinode( $file1) )."\n";
print( fileinode( $file2) )."\n";
clearstatcache();

echo "-- Testing with links: hard link --\n";
link( $file1, $link1);  // Creating an hard link
print( fileinode( $file1) )."\n";
clearstatcache();
print( fileinode( $link1) )."\n";
clearstatcache();

echo "-- Testing with links: soft link --\n";
symlink( $file2, $link2);  // Creating a soft link
print( fileinode( $file2) )."\n";
clearstatcache();
print( fileinode( $link2) )."\n";

unlink( $link1 );
unlink( $link2 );

echo "-- Testing after copying a file --\n";
copy( $file1, $file_path."/fileinode1_variation_new.tmp");
print( fileinode( $file1) )."\n";
clearstatcache();
print( fileinode( $file_path."/fileinode1_variation_new.tmp") )."\n";

unlink( $file_path."/fileinode1_variation_new.tmp");
unlink( $file1);
unlink( $file2);


echo "-- Testing after renaming the file --\n";
fclose( fopen("$file_path/old.txt", "w") );
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
print( fileinode("$file_path/dir/subdir") )."\n";
clearstatcache();

echo "-- Testing with binary input --\n";
print( fileinode(b"$file_path/dir") )."\n";
clearstatcache();
print( fileinode(b"$file_path/dir/subdir") );

rmdir("$file_path/dir/subdir");
rmdir("$file_path/dir");

echo "\n*** Done ***";
--EXPECTF--
*** Testing fileinode() with files, links and directories ***
-- Testing with files --
%i
%i
-- Testing with links: hard link --
%i
%i
-- Testing with links: soft link --
%i
%i
-- Testing after copying a file --
%i
%i
-- Testing after renaming the file --
%i
%i
-- Testing with directories --
%i
%i
-- Testing with binary input --
%i
%i
*** Done ***
