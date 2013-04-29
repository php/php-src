<?php

function get_sysroot() {
	// usually c:\\windows, but not always
	return exec('echo %SYSTEMROOT%');
}

function get_junction(){
	// junction.exe isn't included with Windows
	// its a sysinternals tool for working with filesystem links
	// see: http://technet.microsoft.com/en-us/sysinternals/bb896768
	
	// install somewhere that is on %path% or added to %path%
	return "junction.exe";
}

function get_mountvol() {
	$sysroot = get_sysroot();

	return "$sysroot\\System32\\mountvol.exe";
}

?>
