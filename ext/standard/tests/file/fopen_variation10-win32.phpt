--TEST--
Test fopen() function : variation: interesting paths, no use include path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Run only on Windows");

if (!is_writable('c:\\')) {
    die('skip. C:\\ not writable.');
}

?>
--FILE--
<?php
echo "*** Testing fopen() : variation ***\n";

// fopen with interesting windows paths.
$testdir = __DIR__.'/fopen10.tmpDir';
$rootdir = 'fopen10.tmpdirTwo';
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

$file = "fopen_variation10.tmp";
$firstfile = 'c:\\'.$rootdir.'\\'.$file;
$secondfile = $testdir.'\\'.$file;
$thirdfile = 'c:\\'.$file;

$h = fopen($firstfile, 'w');
fwrite($h, "file in $rootdir");
fclose($h);

$h = fopen($secondfile, 'w');
fwrite($h, "file in fopen10.tmpDir");
fclose($h);

$h = fopen($thirdfile, 'w');
fwrite($h, "file in root");
fclose($h);

foreach($paths as $path) {
      echo "\n--$path--\n";
      $toFind = $path.'\\'.$file;
         $h = fopen($toFind, 'r');
         if ($h === false) {
            echo "file not opened for read\n";
         }
         else {
            fpassthru($h);
            fclose($h);
            echo "\n";
         }
};

unlink($firstfile);
unlink($secondfile);
unlink($thirdfile);
rmdir($testdir);
rmdir('c:\\'.$rootdir);


?>
--EXPECTF--
*** Testing fopen() : variation ***

--c:\--
file in root

--c:--
file in root

--c--

Warning: fopen(c\fopen_variation10.tmp): Failed to open stream: No such file or directory in %s on line %d
file not opened for read

--\--

Warning: fopen(\\fopen_variation10.tmp): Failed to open stream: No such file or directory in %s on line %d
file not opened for read

--/--

Warning: fopen(/\fopen_variation10.tmp): Failed to open stream: No such file or directory in %s on line %d
file not opened for read

--c:fopen10.tmpdirTwo--
file in fopen10.tmpdirTwo

--c:adir--

Warning: fopen(c:adir\fopen_variation10.tmp): Failed to open stream: No such file or directory in %s on line %d
file not opened for read

--c:\/--
file in root

--c:\fopen10.tmpdirTwo\/--
file in fopen10.tmpdirTwo

--c:\fopen10.tmpdirTwo\--
file in fopen10.tmpdirTwo

--c:\fopen10.tmpdirTwo/--
file in fopen10.tmpdirTwo

--%s/fopen10.tmpDir--
file in fopen10.tmpDir

--/sortout--

Warning: fopen(/sortout\fopen_variation10.tmp): Failed to open stream: No such file or directory in %s on line %d
file not opened for read
