--TEST--
Test fopen() function : variation: use include path create and read a file (relative)
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
 * Description: Open a file or a URL and return a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

require_once('fopen_include_path.inc');

echo "*** Testing fopen() : variation ***\n";
$thisTestDir = "fopenVariation16.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$newpath = create_include_path();
set_include_path($newpath);
runtest();
$newpath = generate_next_path();
set_include_path($newpath);
runtest();

teardown_include_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);

function runtest() {
    global $dir1;
       
    $extraDir = "extraDir";

    mkdir($dir1.'/'.$extraDir);
    
	$tmpfile = $extraDir.'/fopen_variation16.tmp';
	$h = fopen($tmpfile, "w+", true);
	fwrite($h, "This is the test file");
	fclose($h);
	
	$h = fopen($dir1.'/'.$tmpfile, "r");
	if ($h === false) {
	   echo "Not created in dir1\n";
	}
	else {
	   echo "created in dir1\n";
	   fclose($h);
	}

	$h = fopen($tmpfile, "r", true);
	if ($h === false) {
	   echo "could not find file for reading\n";
	}
	else {
	   echo "found file again in dir1\n";
	   fclose($h);
	}
	
	unlink($dir1.'/'.$tmpfile);   
    rmdir($dir1.'/'.$extraDir);	
}
?>
===DONE===
--EXPECT--
*** Testing fopen() : variation ***
created in dir1
found file again in dir1
created in dir1
found file again in dir1
===DONE===
