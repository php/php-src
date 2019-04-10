<?php
$thisTestDir = $testName . '.dir';
mkdir($thisTestDir);
chdir($thisTestDir);

//create the include directory structure
$workingDir = "workdir";
$filename = $testName . ".txt.gz";
$scriptDir = dirname(__FILE__);
$baseDir = getcwd();
$secondFile = $baseDir."/dir2/".$filename;
$firstFile = "../dir1/".$filename;
$scriptFile = $scriptDir.'/'.$filename;

$newdirs = array("dir1", "dir2", "dir3");
$pathSep = ":";
$newIncludePath = "";
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
   $pathSep = ";";
}
foreach($newdirs as $newdir) {
   mkdir($newdir);
   $newIncludePath .= '../'.$newdir.$pathSep;
}
mkdir($workingDir);
chdir($workingDir);
?>
