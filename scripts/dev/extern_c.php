<?php

function scan_dir($dir) {
	if (!is_dir($dir)) return;

	foreach (glob("$dir/*") as $file) {
		if (is_dir($file)) {
			if (basename($file) != "CVS") {
				scan_dir($file);
			}
		} else if (fnmatch("*.h", $file)) {
			scan_file($file); 
		}
	}
}
	
function scan_file($file) {
	$flag = false;
	
	foreach (file($file) as $nr => $line) {
		if (ereg("^[[:space:]]*BEGIN_EXTERN_C", $line)) {
#		    echo "$file:".($nr+1)." $line";
			$flag = true;
		} else if (ereg("^[[:space:]]*END_EXTERN_C", $line)) {
#		    echo "$file:".($nr+1)." $line";
			$flag = false;
		} else if (  (ereg("^[[:space:]]*PHPAPI[[:space:]]*", $line)) 
					 ||(ereg("^[[:space:]]*ZEND_API[[:space:]]*", $line))) {
		  if (strstr($line,"(")) { 
			  if (!$flag) echo "$file:".($nr+1)." $line";
		  }
		}
	}
}

array_shift($_SERVER["argv"]);

if (count($_SERVER["argv"])) {
	foreach ($_SERVER["argv"] as $dir) {
		scan_dir($dir);
	}
} else {
	scan_dir(".");
}
?>