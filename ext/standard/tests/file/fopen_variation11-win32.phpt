--TEST--
Test fopen() function : variation: interesting paths, use include path = true
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Run only on Windows");
if (!is_writable('c:\\fopen_variation10.tmp')) {
	die('skip. C:\\ not writable.');
}

?>
--FILE--
<?php
/* Prototype  : resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
 * Description: Open a file or a URL and return a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing fopen() : variation ***\n";

// fopen with interesting windows paths.
$testdir = dirname(__FILE__).'/fopen11.tmpDir';
$rootdir = 'fopen11.tmpdirTwo';
mkdir($testdir);
mkdir('c:\\'.$rootdir);

$unixifiedDir = '/'.substr(str_replace('\\','/',$testdir),3);

$paths = array('c:\\', 
               'c:', 
               'c', 
               '\\', 
               '/', 
               'c:'.$rootdir, 
               'c:adir', 
               'c:\\/', 
               'c:\\'.$rootdir.'\\/',
               'c:\\'.$rootdir.'\\', 
               'c:\\'.$rootdir.'/',
               $unixifiedDir, 
               '/sortout');

$file = "fopen_variation11.tmp";
$firstfile = 'c:\\'.$rootdir.'\\'.$file;
$secondfile = $testdir.'\\'.$file;
$thirdfile = 'c:\\'.$file;

$h = fopen($firstfile, 'w');
fwrite($h, "file in $rootdir");
fclose($h);

$h = fopen($secondfile, 'w');
fwrite($h, "file in fopen11.tmpDir");
fclose($h);

$h = fopen($thirdfile, 'w');
fwrite($h, "file in root");
fclose($h);

foreach($paths as $path) {
      echo "\n--$path--\n";
      $toFind = $path.'\\'.$file;
         $h = fopen($toFind, 'r', true);
         if ($h === false) {
            echo "file not opened for read\n";
         }
         else {
            fpassthru($h);
            echo "\n";
         }
         fclose($h);
};

unlink($firstfile);
unlink($secondfile);
unlink($thirdfile);
rmdir($testdir);
rmdir('c:\\'.$rootdir);

?>
===DONE===
--EXPECTF--
*** Testing fopen() : variation ***

--c:\--
file in root

--c:--
file in root

--c--

Warning: fopen(c\fopen_variation11.tmp): failed to open stream: No such file or directory in %s on line %d
file not opened for read

Warning: fclose() expects parameter 1 to be resource, boolean given in %s on line %d

--\--

Warning: fopen(\\FOPEN_VARIATION11.TMP): failed to open stream: Invalid argument in %s on line %d
file not opened for read

Warning: fclose() expects parameter 1 to be resource, boolean given in %s on line %d

--/--

Warning: fopen(\\FOPEN_VARIATION11.TMP): failed to open stream: Invalid argument in %s on line %d
file not opened for read

Warning: fclose() expects parameter 1 to be resource, boolean given in %s on line %d

--c:fopen11.tmpdirTwo--
file in fopen11.tmpdirTwo

--c:adir--

Warning: fopen(c:adir\fopen_variation11.tmp): failed to open stream: No such file or directory in %s on line %d
file not opened for read

Warning: fclose() expects parameter 1 to be resource, boolean given in %s on line %d

--c:\/--
file in root

--c:\fopen11.tmpdirTwo\/--
file in fopen11.tmpdirTwo

--c:\fopen11.tmpdirTwo\--
file in fopen11.tmpdirTwo

--c:\fopen11.tmpdirTwo/--
file in fopen11.tmpdirTwo

--%s/fopen11.tmpDir--
file in fopen11.tmpDir

--/sortout--

Warning: fopen(/sortout\fopen_variation11.tmp): failed to open stream: No such file or directory in %s on line %d
file not opened for read

Warning: fclose() expects parameter 1 to be resource, boolean given in %s on line %d
===DONE===

