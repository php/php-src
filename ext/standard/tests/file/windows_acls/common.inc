<?php
error_reporting(E_ALL);
define('PHPT_ACL_READ',  1 << 1);
define('PHPT_ACL_WRITE', 1 << 2);
define('PHPT_ACL_EXEC',  1 << 3);
define('PHPT_ACL_NONE',  1 << 4);
define('PHPT_ACL_FULL',  1 << 5);

define('PHPT_ACL_GRANT',  1);
define('PHPT_ACL_DENY',  2);

function skipif() {
	if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
	}
	if(stripos(php_uname(), 'XP') !== FALSE) {
		die('skip windows 2003 or newer only test');
	}
}

function get_username(){
	return getenv('USERNAME');
}

function get_domainname()
{
	return getenv('USERDOMAIN');
}

function icacls_set($path, $mode, $perm) {
	$user = get_username();
	$path_escaped =  '"' . $path . '"';
	$perm_entry = array();

	if ($perm & PHPT_ACL_READ) $perm_entry[]  = 'R';
	if ($perm & PHPT_ACL_WRITE) $perm_entry[] = 'W';
	if ($perm & PHPT_ACL_EXEC) $perm_entry[]  = 'RX';
	if ($perm & PHPT_ACL_FULL) $perm_entry[]  = 'F';

	// Deny all
	$cmd = 'icacls ' . $path_escaped . ' /inheritance:r /deny ' . $user . ':(F,M,R,RX,W)';
	exec($cmd);

	if ($perm & PHPT_ACL_NONE) {
		/*
		 This is required to remove all the previously denied
		 permission for the USER. Just granting permission doesn't
		 remove the previously denied permission.
		*/
		$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:d';
		$cmd .= ' ' . $user;
		exec($cmd);
		$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:g';
		$cmd .= ' ' . $user;
		exec($cmd);
		return;
	}

	if ($mode == PHPT_ACL_GRANT) {
		$mode = 'grant';
	} else {
		$mode = 'deny';
	}


	// Deny all
	$cmd = 'icacls ' . $path_escaped . ' /deny ' . $user . ':(F,M,R,RX,W)';
	exec($cmd);

	/*
	 This is required to remove all the previously denied
	 permission for the USER. Just granting permission doesn't
	 remove the previously denied permission.
	*/
	$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:d';
	$cmd .= ' ' . $user;
	exec($cmd);
	$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:g';
	$cmd .= ' ' . $user;
	exec($cmd);


	/*
	 Required to set no permission and check that is_readable()
	 returns false. If the $perm_entry contains 'N' skip this step.
	 This will make the file/dir with NO aceess.
	*/
	if (!in_array('N', $perm_entry)) {
		/*
		 This is required to remove all the previously denied
		 permission for the USER. Just granting permission doesn't
		 remove the previously denied permission.
		*/
		$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:d';
		$cmd .= ' ' . get_username();
		exec($cmd);
		$cmd = 'icacls ' . $path_escaped . ' /' . 'remove:g';
		$cmd .= ' ' . get_username();
		exec($cmd);

		$cmd = 'icacls ' . $path_escaped . ' /' . $mode;
		$cmd .= ' ' . get_username();
		$cmd .= ':' . '(' . implode($perm_entry, ',') . ')';
		exec($cmd);
	}
}

function create_dir($name, $perms) {
	if (empty($name)) {
		echo "create_dir: Empty name is not allowed\n";
		return;
	}

	mkdir($name);
	$dst = realpath($name);
	icacls_set($name, PHPT_ACL_GRANT, $perms);
}

function create_file($name, $perms) {
	if (empty($name)) {
		echo "create_file: Empty name is not allowed\n";
		return;
	}

	touch($name);
	icacls_set($name, PHPT_ACL_GRANT, $perms);
}

function delete_file($path) {
	icacls_set($path, PHPT_ACL_GRANT, PHPT_ACL_FULL);
	if (is_file($path)) {
		unlink($path);
	} else {
		echo "delete_file: '$path' is not a file\n";
		return;
	}
}

function delete_dir($path) {
	if (is_dir($path)) {
		icacls_set($path, PHPT_ACL_GRANT, PHPT_ACL_FULL);
		rmdir($path);
	} else {
		echo "delete_dir: '$path' is not a directory\n";
		return;
	}
}
if (0) {
$path = __DIR__ . '/a.txt';
create_file($path, PHPT_ACL_NONE);
if (!is_writable($path)) {
	echo "PHPT_ACL_NONE success!!\n";
} else {
	echo "PHPT_ACL_NONE failed!!\n";
}
delete_file($path);

$path = __DIR__ . '/a.txt';
create_file($path, PHPT_ACL_READ);
if (!is_writable($path)) {
	echo "PHPT_ACL_READ success!!\n";
} else {
	echo "PHPT_ACL_READ failed!!\n";
}
delete_file($path);

$path = __DIR__ . '/adir';
create_dir($path, PHPT_ACL_READ);
if (!is_writable($path)) {
	echo "PHPT_ACL_READ dir success!!\n";
} else {
	echo "PHPT_ACL_READ dir failed!!\n";
}
delete_dir($path);

}
