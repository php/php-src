<?php

function get_active_cp()
{
	$s = exec("chcp");

	preg_match(",.*: (\d+),", $s, $m);

	return $m[1];
}

function set_active_cp($cp)
{
	echo exec("chcp $cp"), "\n";
}

function get_basename_with_cp($path, $cp, $echo = true)
{
	$old_cp = get_active_cp();
	set_active_cp($cp);

	if ($echo) echo "getting basename of $pathw\n";

	$cmd = "powershell -command \"Get-Childitem -Path $pathw | Format-Table -HideTableHeaders Name\"";
	$out = trim(shell_exec($cmd));

	if ($echo) var_dump($cmd, $out, $out == basename($pathw));
	if ($echo) var_dump(realpath($pathw));

	set_active_cp($old_cp);

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
	if (!extension_loaded("iconv")) {
		die("skip iconv is not loaded");
	}
}

