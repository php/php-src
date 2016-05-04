<?php

function get_active_cp()
{
	return proc_get_cp();
}

function set_active_cp($cp, $echo = true)
{
	if (!proc_set_cp($cp)) {
		echo "Failed to set cp $cp\n";
		return;
	}

	if ($echo) echo "Active code page: ", get_active_cp(), "\n";
}

function get_basename_with_cp($path, $cp, $echo = true)
{
	$old_cp = get_active_cp();
	set_active_cp($cp, $echo);

	if ($echo) echo "getting basename of $path\n";

	$cmd = "powershell -command \"Get-Item -Path '$path' | Format-Table -HideTableHeaders Name\"";
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

function create_verify_file($prefix, $basename, $content = "", $cp = 65001)
{
	$full = $prefix . DIRECTORY_SEPARATOR . $basename;

	if (!touch($full) || get_basename_with_cp($full, $cp, false) !== $basename) {
		echo "failed to touch create $full";
		return;
	}

	if ($content) {
		file_put_contents($full, $content);
	}
}

function create_verify_dir($prefix, $basename, $cp = 65001)
{
	$full = $prefix . DIRECTORY_SEPARATOR . $basename;

	if (!mkdir($full) || get_basename_with_cp($full, $cp, false) !== $basename) {
		echo "failed to create dir $full";
	}
}

function create_data($id)
{
	$prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . $id;

	$zend_mb = ini_get("zend.multibyte");

	if (!is_dir($prefix)) {
		mkdir($prefix);
	}

	/* Using a UTF-8 encoded filenames if !zend.multibyte, otherwise files in retrospective charset. */
	switch ($id) {

		case "file_mb3":
			$content = "<?php

echo \"hello there from the include\n\";";
			create_verify_file($prefix, "českýtestování.inc", $content);
			break;

		case "bug71509":
			$content = "that's my file with swedish filename
";
			create_verify_file($prefix, "Röd_Statistics.txt", $content);
			break;

		case "file_mb4":
			create_verify_file($prefix, "š.txt", "");
			break;

		case "file_cp1252":
			$content = "hallo
";
			create_verify_file($prefix, "tschüß", $content);
			break;

		case "file2_cp1252":
			create_verify_file($prefix, "Voláçao", "hola");
			break;

		case "file2_mb":
			create_verify_file($prefix, "Ελλάδα.txt", "");
			break;

		case "file_cp1251":
			if ($zend_mb) {
				create_verify_file($prefix, "������", "opened an utf8 filename for reading");
			} else {
				create_verify_file($prefix, "привет", "opened an utf8 filename for reading");
			}
			break;

		case "file_eucjp":
		case "file_mb":
			$content = "reading file wihh eucjp filename
";
			create_verify_file($prefix, "テストマルチバイト・パス", $content);
			break;

		case "file_big5":
			$content = "read from a file with big5 filename";
			create_verify_file($prefix, "測試多字節路徑", $content);
			break;

		case "dir_cp1252":
			create_verify_dir($prefix, "tschüß3");
			break;

		case "dir2_cp1252":
			create_verify_dir($prefix, "Voláçao3");
			break;

		case "dir_cp1251":
			if ($zend_mb) {
				create_verify_dir($prefix, "������3");
			} else {
				create_verify_dir($prefix, "привет3");
			}
			break;

		case "dir_mb":
		case "dir_eucjp":
			create_verify_dir($prefix, "テストマルチバイト・パス42");
			break;

		case "dir_big5":
			create_verify_dir($prefix, "測試多字節路徑5");
			break;
	}

	return $prefix;
}

function remove_data($id, $dir = NULL)
{
	if (!$dir) {
		$dir = dirname(__FILE__) . DIRECTORY_SEPARATOR . $id;
	}

	if (is_dir($dir)) {
		$objects = scandir($dir);
		foreach ($objects as $object) {
			if ($object != "." && $object != "..") {
				if (filetype($dir . DIRECTORY_SEPARATOR . $object) == "dir") 
					remove_data($id, $dir . DIRECTORY_SEPARATOR . $object); 
				else 
					unlink($dir . DIRECTORY_SEPARATOR . $object);
			}
		}
		reset($objects);
		rmdir($dir);
	} 
}
