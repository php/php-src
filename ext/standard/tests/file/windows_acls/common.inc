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
	$user = getenv('USERNAME');

	if (!$user) {
		$user = get_current_user();
	}

	if (!$user) {
		$user =  exec('echo %USERNAME%');
	}

	return $user;
}

function get_domainname()
{
	$domain = getenv('USERDOMAIN');

	return $domain;
}

function get_icacls()
{
	$sysroot = exec('echo %SYSTEMROOT%');

	return "$sysroot\\System32\\icacls.exe";
}

function fix_acls() {
	$user = get_username();
	/* Current user needs to be owner of the test files. As well
	   all the other users having acls on the files must loose them.
	   The following fixes this just partially, as dynamically reading
	   all the users having acls on a file could be sophisticated. */
	exec(get_icacls() . ' . /setowner $user /T /L /Q 2> nul');
	exec(get_icacls() . ' . /remove:g Administrators /T /L /Q 2> nul');
}

function icacls_set($path, $mode, $perm) {
	$icacls = get_icacls();
	$user = get_username();
	$path_escaped =  '"' . $path . '"';
	$perm_entry = array();

	if ($perm & PHPT_ACL_READ) $perm_entry[]  = 'R';
	if ($perm & PHPT_ACL_WRITE) $perm_entry[] = 'W';
	if ($perm & PHPT_ACL_EXEC) $perm_entry[]  = 'RX';
	if ($perm & PHPT_ACL_FULL) $perm_entry[]  = 'F';

	// Deny all
	$cmd = $icacls . ' ' . $path_escaped . ' /inheritance:r /deny ' . $user . ':(F,M,R,RX,W)';
	exec($cmd);

	if ($perm & PHPT_ACL_NONE) {
		/*
		 This is required to remove all the previously denied
		 permission for the USER. Just granting permission doesn't
		 remove the previously denied permission.
		*/
		$cmd = $icacls . ' ' . $path_escaped . ' /remove:d ' . $user;
		exec($cmd);
		$cmd = $icacls . ' ' . $path_escaped . ' /remove:g ' . $user;
		exec($cmd);
		return;
	}

	if ($mode == PHPT_ACL_GRANT) {
		$mode = 'grant';
	} else {
		$mode = 'deny';
	}


	// Deny all
	$cmd = $icacls . ' ' . $path_escaped . ' /deny ' . $user . ':(F,M,R,RX,W)';
	exec($cmd);

	/*
	 This is required to remove all the previously denied
	 permission for the USER. Just granting permission doesn't
	 remove the previously denied permission.
	*/
	$cmd = $icacls . ' ' . $path_escaped . ' /remove:d ' . $user;
	exec($cmd);
	$cmd = $icacls . ' ' . $path_escaped . ' /remove:g ' . $user;
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
		$cmd = $icacls . ' ' . $path_escaped . ' /remove:d ' . $user;
		exec($cmd);
		$cmd = $icacls . ' ' . $path_escaped . ' /remove:g ' . $user;
		exec($cmd);

		$cmd = $icacls . ' ' . $path_escaped . ' /' . $mode . ' ' . $user;
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
