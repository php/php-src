<?php # $Id$
/* piece together a windows binary distro */

$build_dir = $argv[1];
$phpdll = $argv[2];
$sapi_targets = explode(" ", $argv[3]);
$ext_targets = explode(" ", $argv[4]);
$snapshot_template = $argv[5];

$is_debug = preg_match("/^debug/i", $build_dir);

echo "Making dist for $build_dir\n";

$dist_dir = $build_dir . "/php-" . phpversion();
@mkdir($dist_dir);
@mkdir("$dist_dir/sapi");
@mkdir("$dist_dir/ext");
@mkdir("$dist_dir/dev");
@mkdir("$dist_dir/dlls");
@mkdir("$dist_dir/extras");

/* figure out additional DLL's that are required */
$extra_dll_deps = array();
$per_module_deps = array();

function get_depends($module)
{
	static $no_dist = array(
		/* windows system dlls that should not be bundled */
		'advapi32.dll', 'comdlg32.dll', 'gdi32.dll', 'kernel32.dll', 'ntdll.dll',
		'odbc32.dll', 'ole32.dll', 'oleaut32.dll', 'rpcrt4.dll',
		'shell32.dll', 'shlwapi.dll', 'user32.dll', 'ws2_32.dll', 'ws2help.dll',
		'comctl32.dll', 'winmm.dll', 'wsock32.dll',

		/* apache */
		'apachecore.dll',

		/* visual C++; mscvrt.dll is present on everyones system,
		 * but the debug version (msvcrtd.dll) and those from visual studio.net
		 * (msvcrt7x.dll) are not */
		'msvcrt.dll',

		);
	global $build_dir, $extra_dll_deps, $ext_targets, $sapi_targets, $phpdll, $per_module_deps;
	
	$bd = strtolower(realpath($build_dir));

	$csvname = tempnam($GLOBALS['build_dir'], '_dep');
	system("depends.exe /c /f:1 /oc:\"$csvname\" \"$module\"", $retcode);
	$fp = fopen($csvname, "r");
	$headers = fgetcsv($fp);
	$n = 0;
	while (($line = fgetcsv($fp))) {
		$n++;
		if ($line[0] == 'D')
			continue;

		$dep = strtolower($line[1]);
		$depbase = basename($dep);
		/* ignore stuff in our build dir, but only if it is
	     * on of our targets */
		if (0 == strncmp($dep, $bd, strlen($bd)) &&
				(in_array($depbase, $sapi_targets) ||
			   	in_array($depbase, $ext_targets)) ||
				$depbase == $phpdll) {
			continue;
		}
		/* ignore some well-known system dlls */
		if (in_array(basename($dep), $no_dist)) {
			continue;
		}
		
		if (!in_array($dep, $extra_dll_deps)) {
			$extra_dll_deps[] = $dep;
		}

		$per_module_deps[basename($module)][] = $dep;
	}
	fclose($fp);
	unlink($csvname);
//echo "Module $module [$n lines]\n";
}

function copy_file_list($source_dir, $dest_dir, $list)
{
	global $is_debug, $dist_dir;

	foreach ($list as $item) {
		echo "Copying $item from $source_dir to $dest_dir\n";
		copy($source_dir . DIRECTORY_SEPARATOR . $item, $dest_dir . DIRECTORY_SEPARATOR . $item);
		if ($is_debug) {
			$itemdb = preg_replace("/\.(exe|dll)$/i", ".pdb", $item);
			if (file_exists("$source_dir/$itemdb")) {
				copy("$source_dir/$itemdb", "$dist_dir/dev/$itemdb");
			}
		}
		if (preg_match("/\.(exe|dll)$/i", $item)) {
			get_depends($source_dir . '/' . $item);
		}
	}
}

function copy_text_file($source, $dest)
{
	$text = file_get_contents($source);
	$text = str_replace("\n", "\r\n", $text);
	$fp = fopen($dest, "w");
	fwrite($fp, $text);
	fclose($fp);
}

/* the core dll */
copy("$build_dir/php.exe", "$dist_dir/php.exe");
copy("$build_dir/$phpdll", "$dist_dir/$phpdll");

/* and the .lib goes into dev */
$phplib = str_replace(".dll", ".lib", $phpdll);
copy("$build_dir/$phplib", "$dist_dir/dev/$phplib");
/* debug builds; copy the symbols too */
if ($is_debug) {
	$phppdb = str_replace(".dll", ".pdb", $phpdll);
	copy("$build_dir/$phppdb", "$dist_dir/dev/$phppdb");
}
/* copy the sapi */
copy_file_list($build_dir, "$dist_dir/sapi", $sapi_targets);

/* copy the extensions */
copy_file_list($build_dir, "$dist_dir/ext", $ext_targets);

/* populate reading material */
$text_files = array(
	"LICENSE" => 		"license.txt",
	"NEWS" => 			"news.txt",
	"php.ini-dist" => 	"php.ini-dist",
	"php.ini-recommended" => "php.ini-recommended",
);

foreach ($text_files as $src => $dest) {
	copy_text_file($src, $dist_dir . '/' . $dest);
}

/* general other files */
$general_files = array(
	"win32/install.txt" => 	"install.txt",
	"php.gif"			=>	"php.gif",
);

foreach ($general_files as $src => $dest) {
	copy($src, $dist_dir . '/' . $dest);
}

/* include a snapshot identifier */
$branch = "HEAD"; // TODO - determine this from CVS/Entries 
$fp = fopen("$dist_dir/snapshot.txt", "w");
$now = date("r");
$version = phpversion();
fwrite($fp, <<<EOT
This snapshot was automatically generated on
$now

Version: $version
Branch: $branch
Build: $build_dir

EOT
);
/* list build-in extensions */
$exts = get_loaded_extensions();
fprintf($fp, "\r\nBuilt-in Extensions\r\n");
fwrite($fp, "===========================\r\n");
foreach ($exts as $ext) {
	fprintf($fp, "%s\r\n", $ext);
}
fwrite($fp, "\r\n\r\n");

/* list dependencies */
fprintf($fp, "Dependency information:\r\n");
foreach ($per_module_deps as $modulename => $deps) {
	fprintf($fp, "Module: %s\r\n", $modulename);
	fwrite($fp, "===========================\r\n");
	foreach ($deps as $dll) {
		fprintf($fp, "\t%s\r\n", basename($dll));
	}
	fwrite($fp, "\r\n");
}
fclose($fp);

/* Now add those dependencies */
foreach ($extra_dll_deps as $dll) {
	if (!file_exists($dll)) {
		/* try template dir */
		$tdll = $snapshot_template . "/dlls/" . basename($dll);
		if (!file_exists($tdll)) {
			echo "WARNING: distro depends on $dll, but could not find it on your system\n";
			continue;
		}
		$dll = $tdll;
	}
	copy($dll, "$dist_dir/dlls/" . basename($dll));
}

function copy_dir($source, $dest)
{
	if (!is_dir($dest)) {
		if (!mkdir($dest)) {
			return false;
		}
	}

	$d = opendir($source);
	while (($f = readdir($d)) !== false) {
		if ($f == '.' || $f == '..' || $f == 'CVS') {
			continue;
		}
		$fs = $source . '/' . $f;
		$fd = $dest . '/' . $f;
		if (is_dir($fs)) {
			copy_dir($fs, $fd);
		} else {
			copy($fs, $fd);
		}
	}
	closedir($d);
}

/* add extras from the template dir */
if (file_exists($snapshot_template)) {
	$items = glob("$snapshot_template/*");
	print_r($items);

	foreach ($items as $item) {
		$bi = basename($item);
		if (is_dir($item)) {
			if ($bi == 'dlls' || $bi == 'symbols') {
				continue;
			} else if ($bi == 'PEAR') {
				/* copy to top level */
				copy_dir($item, "$dist_dir/$bi");
			} else {
				/* copy that dir into extras */
				copy_dir($item, "$dist_dir/extras/$bi");
			}
		} else {
			if ($bi == 'go-pear.bat') {
				/* copy to top level */
				copy($item, "$dist_dir/$bi");
			} else {
				/* copy to extras */
				copy($item, "$dist_dir/extras/$bi");
			}
		}
	}
} else {
	echo "WARNING: you don't have a snapshot template\n";
	echo "         your dist will not be complete\n";
}

?>
