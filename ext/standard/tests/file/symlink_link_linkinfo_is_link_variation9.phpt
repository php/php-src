--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - link & lstat[dev] value
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    require_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
/* Variation 9 : Check lstat of soft/hard link created
                 Check linkinfo() value with lstat['dev']
*/
// temp file used
$file_path = __DIR__;
$dirname = $file_path."/symlink_link_linkinfo_is_link_variation9";
mkdir($dirname);
$filename = "$dirname/symlink_link_linkinfo_is_link_variation9.tmp";
// soft link name used
$soft_link = "$dirname/symlink_link_linkinfo_is_link_softlink_variation9.tmp";
// hard link name used
$hard_link = "$dirname/symlink_link_linkinfo_is_link_hardlink_variation9.tmp";

// create the file
$fp = fopen($filename, "w");
fclose($fp);

echo "*** Checking lstat() on soft link ***\n";
// create soft link
var_dump( symlink($filename, $soft_link) );

// confirming that linkinfo() = lstat['dev'] , this should always match
$linkinfo = linkinfo($soft_link);
$s1 = lstat($soft_link);
echo "linkinfo() returns integer !== -1: ";
var_dump(is_int($linkinfo) && $linkinfo !== -1);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesn't match lstat['dev']\n";
// delete link
unlink($soft_link);

echo "\n*** Checking lstat() on hard link ***\n";
// create hard link
var_dump( link($filename, $hard_link) );
// confirming that linkinfo() = lstat['dev'] , this should always match
$linkinfo = linkinfo($hard_link);
$s1 = lstat($hard_link);
echo "linkinfo() returns integer !== -1: ";
var_dump(is_int($linkinfo) && $linkinfo !== -1);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesn't match lstat['dev']\n";

// delete link
unlink($hard_link);

echo "\n*** Checking lstat() on a soft link to directory ***\n";
// create soft link
var_dump( symlink($dirname, $soft_link) );

// confirming that linkinfo() = lstat['dev'], this should always match
$linkinfo = linkinfo($soft_link);
$s1 = lstat($soft_link);
echo "linkinfo() returns integer !== -1: ";
var_dump(is_int($linkinfo) && $linkinfo !== -1);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesn't match lstat['dev']\n";

// delete link
if (PHP_OS_FAMILY === 'Windows') {
  rmdir($soft_link);
} else {
  unlink($soft_link);
}

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dirname = $file_path."/symlink_link_linkinfo_is_link_variation9";
$filename = "$dirname/symlink_link_linkinfo_is_link_variation9.tmp";
unlink($filename);
rmdir($dirname);
?>
--EXPECT--
*** Checking lstat() on soft link ***
bool(true)
linkinfo() returns integer !== -1: bool(true)

linkinfo() value matches lstat['dev']

*** Checking lstat() on hard link ***
bool(true)
linkinfo() returns integer !== -1: bool(true)

linkinfo() value matches lstat['dev']

*** Checking lstat() on a soft link to directory ***
bool(true)
linkinfo() returns integer !== -1: bool(true)

linkinfo() value matches lstat['dev']
Done
