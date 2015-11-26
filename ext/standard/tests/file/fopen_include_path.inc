<?php
$pwd = getcwd();
$f = basename(current(get_included_files()), ".php");
$dir1 = $pwd."/".$f.".dir1";
$dir2 = $pwd."/".$f.".dir2";
$dir3 = $pwd."/".$f.".dir3";
//invalid directory
$dir4 = $pwd."/".$f.".dir4";
$newdirs = array($dir1, $dir2, $dir3);

$reldirs = array("dir1", "dir2", "dir3");

function generate_next_rel_path() {
	global $reldirs;
	//create the include directory structure
	$pathSep = ":";
	$newIncludePath = "";
	if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	   $pathSep = ";";
	}
	foreach($reldirs as $newdir) {
	   $newIncludePath .= $newdir.$pathSep;
	}
	return "dir4".$pathSep . $newIncludePath;
}

function generate_next_path() {
	global $newdirs, $dir4;
	//create the include directory structure
	$pathSep = ":";
	$newIncludePath = "";
	if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	   $pathSep = ";";
	}
	foreach($newdirs as $newdir) {
	   $newIncludePath .= $newdir.$pathSep;
	}
	return $dir4.$pathSep . $newIncludePath;
}


function create_include_path() {

	global $newdirs;
	//create the include directory structure
	$pathSep = ":";
	$newIncludePath = "";
	if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	   $pathSep = ";";
	}
	foreach($newdirs as $newdir) {
	   mkdir($newdir);
	   $newIncludePath .= $newdir.$pathSep;
	}
	return $newIncludePath;
}

function relative_include_path() {

	global $reldirs;
	//create the include directory structure
	$pathSep = ":";
	$newIncludePath = "";
	if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	   $pathSep = ";";
	}
	foreach($reldirs as $newdir) {
	   mkdir($newdir);
	   $newIncludePath .= $newdir.$pathSep;
	}
	return $newIncludePath;
}


function teardown_include_path() {

	global $newdirs;
	// remove the directory structure
	foreach($newdirs as $newdir) {
	   rmdir($newdir);
	}
}

function teardown_relative_path() {

	global $reldirs;
	// remove the directory structure
	foreach($reldirs as $newdir) {
	   rmdir($newdir);
	}
}
?>