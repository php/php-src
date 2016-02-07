<?php

function get_active_cp()
{
	$s = exec("chcp");

	preg_match(",.*: (\d+),", $s, $m);

	return $m[1];
}

function set_active_cp($cp, $echo = true)
{
	$ret = exec("chcp $cp");

	if ($echo) echo "Active code page: ", get_active_cp(), "\n";
}

function get_basename_with_cp($path, $cp, $echo = true)
{
	$old_cp = get_active_cp();
	set_active_cp($cp, $echo);

	if ($echo) echo "getting basename of $path\n";

	$cmd = "powershell -command \"Get-Item -Path $path | Format-Table -HideTableHeaders Name\"";
	$out = trim(shell_exec($cmd));

	if ($echo) var_dump($out, $out == basename($path));
	if ($echo) var_dump(realpath($path));

	set_active_cp($old_cp, $echo);

	return $out;
}

function skip_if_wrong_cp($cp)
{
	if (get_active_cp() != $cp) {
		die("skip this test expect codepage $cp");
	}
}

function skip_if_no_required_exts()
{
	$exts = func_get_args();
	$exts[] = "iconv";

	foreach ($exts as $ext) {
		if (!extension_loaded($ext)) {
			die("skip $ext is not loaded");
		}
	}
}

function skip_if_not_win()
{
	if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
	}
}

