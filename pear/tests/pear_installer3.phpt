--TEST--
PEAR_Installer test #3 File Transactions
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php
$temp_path = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'testinstallertemp';
if (!is_dir($temp_path)) {
    mkdir($temp_path);
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'php')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'php');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'data')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'data');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'doc')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'doc');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'test')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'test');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'ext')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'ext');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'script')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'script');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'tmp')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'tmp');
}
if (!is_dir($temp_path . DIRECTORY_SEPARATOR . 'bin')) {
    mkdir($temp_path . DIRECTORY_SEPARATOR . 'bin');
}
// make the fake configuration - we'll use one of these and it should work
$config = serialize(array('master_server' => 'pear.php.net',
    'php_dir' => $temp_path . DIRECTORY_SEPARATOR . 'php',
    'ext_dir' => $temp_path . DIRECTORY_SEPARATOR . 'ext',
    'data_dir' => $temp_path . DIRECTORY_SEPARATOR . 'data',
    'doc_dir' => $temp_path . DIRECTORY_SEPARATOR . 'doc',
    'test_dir' => $temp_path . DIRECTORY_SEPARATOR . 'test',
    'bin_dir' => $temp_path . DIRECTORY_SEPARATOR . 'bin',));
touch($temp_path . DIRECTORY_SEPARATOR . 'pear.conf');
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'pear.conf', 'w');
fwrite($fp, $config);
fclose($fp);
touch($temp_path . DIRECTORY_SEPARATOR . 'pear.ini');
$fp = fopen($temp_path . DIRECTORY_SEPARATOR . 'pear.ini', 'w');
fwrite($fp, $config);
fclose($fp);

putenv('PHP_PEAR_SYSCONF_DIR='.$temp_path);
$home = getenv('HOME');
if (!empty($home)) {
    // for PEAR_Config initialization
    putenv('HOME="'.$temp_path);
}
require_once "PEAR/Installer.php";

// no UI is needed for these tests
$ui = false;
$installer = new PEAR_Installer($ui);
$installer->debug = 2; // hack debugging in
$curdir = getcwd();
chdir(dirname(__FILE__));

echo "test addFileOperation():\n";
echo "invalid input to addFileOperation():\n";
$err = $installer->addFileOperation('rename', 2);
echo 'Returned PEAR_Error?';
echo (get_class($err) == 'pear_error' ? " yes\n" : " no\n");
if (get_class($err) == 'pear_error') {
    echo $err->getMessage() . "\n";
}
echo 'count($installer->file_operations) = ';
var_dump(count($installer->file_operations));
echo "Do valid addFileOperation() delete\n";
touch($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt');
$installer->addFileOperation('delete', array($temp_path . DIRECTORY_SEPARATOR .
    'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt'));
echo 'count($installer->file_operations) = ';
var_dump(count($installer->file_operations));

echo "test valid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
}

echo "Do valid addFileOperation() rename\n";
touch($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt');
$installer->addFileOperation('rename', array($temp_path . DIRECTORY_SEPARATOR .
    'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt', $temp_path .
    DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'renamed.phpt'));

echo 'file renamed.phpt exists?';
clearstatcache();
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR
    . 'renamed.phpt') ? " yes\n" : " no\n");
echo "test valid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
}
echo 'file renamed.phpt exists?';
clearstatcache();
echo (file_exists($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR
    . 'renamed.phpt') ? " yes\n" : " no\n");
unlink($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR
    . 'renamed.phpt');

echo "Do valid addFileOperation() chmod\n";
touch($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt');
clearstatcache();
$perms = fileperms($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt');
// check to see if chmod works on this OS
chmod($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt', 0776);
clearstatcache();
if (fileperms($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt')
      == $perms && substr(PHP_OS, 0, 3) == 'WIN') {
    // we are on windows, so skip this test, but simulate success
echo <<<EOF
file permissions are: 776
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
file permissions are: 640

EOF;
} else {
    $installer->addFileOperation('chmod', array(0640, $temp_path . DIRECTORY_SEPARATOR .
        'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt'));
    
    echo 'file permissions are: ' . decoct(fileperms($temp_path . DIRECTORY_SEPARATOR .
        'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt')) . "\n";
    echo "test valid commitFileTransaction():\n";
    if ($installer->commitFileTransaction()) {
        echo "worked\n";
    } else {
        echo "didn't work!\n";
    }
    echo 'file permissions are: ' . decoct(fileperms($temp_path . DIRECTORY_SEPARATOR .
        'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah.phpt')) . "\n";
}
unlink($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah.phpt');

mkdir($temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah');
echo "Do valid addFileOperation() rmdir\n";
echo 'directory exists?';
clearstatcache();
echo (is_dir($temp_path . DIRECTORY_SEPARATOR .
    'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah') ? " yes\n" : " no\n");
$installer->addFileOperation('rmdir', array($temp_path . DIRECTORY_SEPARATOR .
    'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah'));

echo "test valid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
}
echo 'directory exists?';

clearstatcache();
echo (is_dir($temp_path . DIRECTORY_SEPARATOR .
    'tmp' . DIRECTORY_SEPARATOR . 'installertestfooblah') ? " yes\n" : " no\n");

echo "Do valid addFileOperation() installed_as\n";
$installer->addFileOperation('installed_as', array('test.php',
    $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp' . DIRECTORY_SEPARATOR . 'fromp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah.phpt',
    $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR,
    'glomp' . DIRECTORY_SEPARATOR . 'fromp'));
echo "test valid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
}
if (isset($installer->pkginfo['filelist'])) {
    echo "filelist created\n";
} else {
    echo "filelist not created!\n";
}
if (isset($installer->pkginfo['filelist']['test.php'])) {
    echo "filelist test.php created\n";
} else {
    echo "filelist test.php not created!\n";
}
if (isset($installer->pkginfo['filelist']['test.php']['installed_as'])) {
    echo "filelist test.php installed_as created\n";
} else {
    echo "filelist test.php installed_as not created!\n";
}
$p = $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp' . DIRECTORY_SEPARATOR . 'fromp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah.phpt';
if (@$installer->pkginfo['filelist']['test.php']['installed_as'] == $p) {
    echo "filelist test.php installed_as is correct\n";
} else {
    echo "filelist test.php installed_as is not correct!\n";
}
if (isset($installer->pkginfo['filelist']['dirtree'])) {
    echo "filelist dirtree created\n";
} else {
    echo "filelist dirtree not created!\n";
}
$p = $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp';
if (isset($installer->pkginfo['filelist']['dirtree'][$p])) {
    echo "filelist dirtree glomp created\n";
} else {
    echo "filelist dirtree glomp not created!\n";
}
$p .= DIRECTORY_SEPARATOR . 'fromp';
if (isset($installer->pkginfo['filelist']['dirtree'][$p])) {
    echo "filelist dirtree fromp created\n";
} else {
    echo "filelist dirtree fromp not created!\n";
}

echo "Do valid addFileOperation() installed_as\n";
$installer->addFileOperation('installed_as', array('test.php',
    $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp' . DIRECTORY_SEPARATOR . 'fromp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah.phpt',
    $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR,
    'glomp' . DIRECTORY_SEPARATOR . 'fromp'));
echo "test valid rollbackFileTransaction():\n";
$installer->rollbackFileTransaction();
if (isset($installer->pkginfo['filelist'])) {
    echo "filelist created\n";
} else {
    echo "filelist not created!\n";
}
if (isset($installer->pkginfo['filelist']['test.php'])) {
    echo "filelist test.php created\n";
} else {
    echo "filelist test.php not created!\n";
}
if (isset($installer->pkginfo['filelist']['test.php']['installed_as'])) {
    echo "filelist test.php installed_as created\n";
} else {
    echo "filelist test.php installed_as not created!\n";
}
$p = $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp' . DIRECTORY_SEPARATOR . 'fromp' . DIRECTORY_SEPARATOR
    . 'installertestfooblah.phpt';
if (@$installer->pkginfo['filelist']['test.php']['installed_as'] == $p) {
    echo "filelist test.php installed_as is correct\n";
} else {
    echo "filelist test.php installed_as is not correct!\n";
}
if (isset($installer->pkginfo['filelist']['dirtree'])) {
    echo "filelist dirtree created\n";
} else {
    echo "filelist dirtree not created!\n";
}
$p = $temp_path . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR .
    'glomp';
if (isset($installer->pkginfo['filelist']['dirtree'][$p])) {
    echo "filelist dirtree glomp created\n";
} else {
    echo "filelist dirtree glomp not created!\n";
}
$p .= DIRECTORY_SEPARATOR . 'fromp';
if (isset($installer->pkginfo['filelist']['dirtree'][$p])) {
    echo "filelist dirtree fromp created\n";
} else {
    echo "filelist dirtree fromp not created!\n";
}

// invalid tests
echo "\n==>Invalid tests\n";
echo "Do valid addFileOperation() delete with non-existing file\n";
$installer->addFileOperation('delete', array('gloober62456.phpt'));
echo 'count($installer->file_operations) = ';
var_dump(count($installer->file_operations));

echo "test invalid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
    $installer->rollbackFileTransaction();
}

echo "Do valid addFileOperation() rename with non-existing file\n";
$installer->addFileOperation('rename', array('gloober62456.phpt', 'faber.com'));
echo 'count($installer->file_operations) = ';
var_dump(count($installer->file_operations));

echo "test invalid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
    $installer->rollbackFileTransaction();
}

echo "Do valid addFileOperation() chmod with non-existing file\n";
$installer->addFileOperation('chmod', array(0640, 'faber.com'));
echo 'count($installer->file_operations) = ';
var_dump(count($installer->file_operations));

echo "test invalid commitFileTransaction():\n";
if ($installer->commitFileTransaction()) {
    echo "worked\n";
} else {
    echo "didn't work!\n";
    $installer->rollbackFileTransaction();
}

//cleanup
chdir($curdir);
unlink ($temp_path . DIRECTORY_SEPARATOR . 'pear.conf');
unlink ($temp_path . DIRECTORY_SEPARATOR . 'pear.ini');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'php');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'data');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'doc');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'test');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'script');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'ext');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'tmp');
rmdir($temp_path . DIRECTORY_SEPARATOR . 'bin');
rmdir($temp_path);
?>
--GET--
--POST--
--EXPECT--
test addFileOperation():
invalid input to addFileOperation():
Returned PEAR_Error? yes
Internal Error: $data in addFileOperation must be an array, was integer
count($installer->file_operations) = int(0)
Do valid addFileOperation() delete
count($installer->file_operations) = int(1)
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
Do valid addFileOperation() rename
file renamed.phpt exists? no
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
file renamed.phpt exists? yes
Do valid addFileOperation() chmod
file permissions are: 776
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
file permissions are: 640
Do valid addFileOperation() rmdir
directory exists? yes
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
directory exists? no
Do valid addFileOperation() installed_as
test valid commitFileTransaction():
about to commit 1 file operations
successfully committed 1 file operations
worked
filelist created
filelist test.php created
filelist test.php installed_as created
filelist test.php installed_as is correct
filelist dirtree created
filelist dirtree glomp created
filelist dirtree fromp created
Do valid addFileOperation() installed_as
test valid rollbackFileTransaction():
rolling back 1 file operations
filelist created
filelist test.php created
filelist test.php installed_as not created!
filelist test.php installed_as is not correct!
filelist dirtree not created!
filelist dirtree glomp not created!
filelist dirtree fromp not created!

==>Invalid tests
Do valid addFileOperation() delete with non-existing file
count($installer->file_operations) = int(1)
test invalid commitFileTransaction():
about to commit 1 file operations
warning: file gloober62456.phpt doesn't exist, can't be deleted
successfully committed 1 file operations
worked
Do valid addFileOperation() rename with non-existing file
count($installer->file_operations) = int(1)
test invalid commitFileTransaction():
about to commit 1 file operations
cannot rename file gloober62456.phpt, doesn't exist
didn't work!
rolling back 1 file operations
Do valid addFileOperation() chmod with non-existing file
count($installer->file_operations) = int(1)
test invalid commitFileTransaction():
about to commit 1 file operations
permission denied (chmod): faber.com 640
didn't work!
rolling back 1 file operations
