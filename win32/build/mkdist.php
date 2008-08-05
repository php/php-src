<?php # $Id$
/* piece together a windows binary distro */

$build_dir = $argv[1];
$phpdll = $argv[2];
$sapi_targets = explode(" ", $argv[3]);
$ext_targets = explode(" ", $argv[4]);
$pecl_targets = explode(" ", $argv[5]);
$snapshot_template = $argv[6];

$is_debug = preg_match("/^debug/i", $build_dir);

echo "Making dist for $build_dir\n";

$dist_dir = $build_dir . "/php-" . phpversion();
$pecl_dir = $build_dir . "/pecl-" . phpversion();

@mkdir($dist_dir);
@mkdir("$dist_dir/ext");
@mkdir("$dist_dir/dev");
@mkdir("$dist_dir/extras");
@mkdir($pecl_dir);

/* figure out additional DLL's that are required */
$extra_dll_deps = array();
$per_module_deps = array();
$pecl_dll_deps = array();

function get_depends($module)
{
	static $no_dist = array(
		/* windows system dlls that should not be bundled */
		'advapi32.dll', 'comdlg32.dll', 'crypt32.dll', 'gdi32.dll', 'kernel32.dll', 'ntdll.dll',
		'odbc32.dll', 'ole32.dll', 'oleaut32.dll', 'rpcrt4.dll',
		'shell32.dll', 'shlwapi.dll', 'user32.dll', 'ws2_32.dll', 'ws2help.dll',
		'comctl32.dll', 'winmm.dll', 'wsock32.dll', 'winspool.drv', 'msasn1.dll',
		'secur32.dll', 'netapi32.dll',

		/* apache */
		'apachecore.dll',

		/* apache 2 */
		'libhttpd.dll', 'libapr.dll', 'libaprutil.dll','libapr-1.dll', 'libaprutil-1.dll',

		/* pi3web */
		'piapi.dll', 'pi3api.dll',

		/* nsapi */
		'ns-httpd30.dll', 'ns-httpd35.dll', 'ns-httpd36.dll', 'ns-httpd40.dll',

		/* oracle */
		'oci.dll', 'ociw32.dll',

		/* sybase */
		'libcs.dll', 'libct.dll',

		/* visual C++; mscvrt.dll is present on everyones system,
		 * but the debug version (msvcrtd.dll) and those from visual studio.net
		 * (msvcrt7x.dll) are not */
		'msvcrt.dll',

		);
	global $build_dir, $extra_dll_deps, $ext_targets, $sapi_targets, $pecl_targets, $phpdll, $per_module_deps, $pecl_dll_deps;
	
	$bd = strtolower(realpath($build_dir));

	$is_pecl = in_array($module, $pecl_targets);
	
	$cmd = "$GLOBALS[build_dir]\\deplister.exe \"$module\" \"$GLOBALS[build_dir]\"";
	$proc = proc_open($cmd, 
			array(1 => array("pipe", "w")),
			$pipes);

	$n = 0;
	while (($line = fgetcsv($pipes[1]))) {
		$n++;

		$dep = strtolower($line[0]);
		$depbase = basename($dep);
		/* ignore stuff in our build dir, but only if it is
	     * one of our targets */
		if (((in_array($depbase, $sapi_targets) ||
			   	in_array($depbase, $ext_targets) || in_array($depbase, $pecl_targets)) ||
				$depbase == $phpdll) && file_exists($GLOBALS['build_dir'] . "/$depbase")) {
			continue;
		}
		/* ignore some well-known system dlls */
		if (in_array(basename($dep), $no_dist)) {
			continue;
		}
		
		if ($is_pecl) {
			if (!in_array($dep, $pecl_dll_deps)) {
				$pecl_dll_deps[] = $dep;
			}
		} else {
			if (!in_array($dep, $extra_dll_deps)) {
				$extra_dll_deps[] = $dep;
			}
		}

		$per_module_deps[basename($module)][] = $dep;
	}
	fclose($pipes[1]);
	proc_close($proc);
//echo "Module $module [$n lines]\n";
}

function copy_file_list($source_dir, $dest_dir, $list)
{
	global $is_debug, $dist_dir;

	foreach ($list as $item) {
		echo "Copying $item from $source_dir to $dest_dir\n";
		copy($source_dir . DIRECTORY_SEPARATOR . $item, $dest_dir . DIRECTORY_SEPARATOR . $item);
		if ($is_debug) {
			$itemdb = preg_replace("/\.(exe|dll|lib)$/i", ".pdb", $item);
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
	$text = preg_replace("/(\r\n?)|\n/", "\r\n", $text);
	$fp = fopen($dest, "w");
	fwrite($fp, $text);
	fclose($fp);
}

/* very light-weight function to extract a single named file from
 * a gzipped tarball.  This makes assumptions about the files
 * based on the PEAR info set in $packages. */
function extract_file_from_tarball($pkg, $filename, $dest_dir) /* {{{ */
{
	global $packages;

	$name = $pkg . '-' . $packages[$pkg];
	$tarball = $dest_dir . "/" . $name . '.tgz';
	$filename = $name . '/' . $filename;
	$destfilename = $dest_dir . "/" . basename($filename);

	$fp = gzopen($tarball, 'rb');

	$done = false;
	do {
		/* read the header */
		$hdr_data = gzread($fp, 512);
	   	if (strlen($hdr_data) == 0)
			break;
		$checksum = 0;
		for ($i = 0; $i < 148; $i++)
			$checksum += ord($hdr_data{$i});
		for ($i = 148; $i < 156; $i++)
			$checksum += 32;
		for ($i = 156; $i < 512; $i++)
			$checksum += ord($hdr_data{$i});

		$hdr = unpack("a100filename/a8mode/a8uid/a8gid/a12size/a12mtime/a8checksum/a1typeflag/a100link/a6magic/a2version/a32uname/a32gname/a8devmajor/a8devminor", $hdr_data);

		$hdr['checksum'] = octdec(trim($hdr['checksum']));

		if ($hdr['checksum'] != $checksum) {
			echo "Checksum for $tarball $hdr[filename] is invalid\n";
			print_r($hdr);
			return;
		}

		$hdr['size'] = octdec(trim($hdr['size']));
		echo "File: $hdr[filename] $hdr[size]\n";
		
		if ($filename == $hdr['filename']) {
			echo "Found the file we want\n";
			$dest = fopen($destfilename, 'wb');
			$x = stream_copy_to_stream($fp, $dest, $hdr['size']);
			fclose($dest);
			echo "Wrote $x bytes into $destfilename\n";
			break;
		}
		
		/* skip body of the file */
		$size = 512 * ceil((int)$hdr['size'] / 512);
		echo "Skipping $size bytes\n";
		gzseek($fp, gztell($fp) + $size);
		
	} while (!$done);
	
} /* }}} */


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
copy_file_list($build_dir, "$dist_dir", $sapi_targets);

/* copy the extensions */
copy_file_list($build_dir, "$dist_dir/ext", $ext_targets);

/* pecl sapi and extensions */
copy_file_list($build_dir, $pecl_dir, $pecl_targets);

/* populate reading material */
$text_files = array(
	"LICENSE" => 		"license.txt",
	"NEWS" => 			"news.txt",
	"php.ini-dist" => 	"php.ini-dist",
	"php.ini-recommended" => "php.ini-recommended",
	"win32/install.txt" => 	"install.txt",
	"win32/pws-php5cgi.reg" => "pws-php5cgi.reg",
	"win32/pws-php5isapi.reg" => "pws-php5isapi.reg",
);

foreach ($text_files as $src => $dest) {
	copy_text_file($src, $dist_dir . '/' . $dest);
}

/* general other files */
$general_files = array(
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
	if (in_array($modulename, $pecl_targets))
		continue;

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
			$tdll = '../deps/bin/' . basename($dll);
			if (!file_exists($tdll)) {
				echo "WARNING: distro depends on $dll, but could not find it on your system\n";
				continue;
			}
		}
		$dll = $tdll;
	}
	copy($dll, "$dist_dir/" . basename($dll));
}
/* and those for pecl */
foreach ($pecl_dll_deps as $dll) {
	if (in_array($dll, $extra_dll_deps)) {
		/* already in main distro */
		continue;
	}
	if (!file_exists($dll)) {
		/* try template dir */
		$tdll = $snapshot_template . "/dlls/" . basename($dll);
		if (!file_exists($tdll)) {
			echo "WARNING: distro depends on $dll, but could not find it on your system\n";
			continue;
		}
		$dll = $tdll;
	}
	copy($dll, "$pecl_dir/" . basename($dll));
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

/* change this next line to true to use good-old
 * hand-assembled go-pear-bundle from the snapshot template */
$use_pear_template = true;

if (!$use_pear_template) {
	/* Let's do a PEAR-less pear setup */
	mkdir("$dist_dir/PEAR");
	mkdir("$dist_dir/PEAR/go-pear-bundle");

	/* grab the bootstrap script */
	echo "Downloading go-pear\n";
	copy("http://pear.php.net/go-pear", "$dist_dir/PEAR/go-pear.php");

	/* import the package list -- sets $packages variable */
	include "pear/go-pear-list.php";

	/* download the packages into the destination */
	echo "Fetching packages\n";

	foreach ($packages as $name => $version) {
		$filename = "$name-$version.tgz";
		$destfilename = "$dist_dir/PEAR/go-pear-bundle/$filename";
		if (file_exists($destfilename))
			continue;
		$url = "http://pear.php.net/get/$filename";
		echo "Downloading $name from $url\n";
		flush();
		copy($url, $destfilename);
	}

	echo "Download complete.  Extracting bootstrap files\n";

	/* Now, we want PEAR.php, Getopt.php (Console_Getopt) and Tar.php (Archive_Tar)
	 * broken out of the tarballs */
	extract_file_from_tarball('PEAR', 'PEAR.php', "$dist_dir/PEAR/go-pear-bundle");
	extract_file_from_tarball('Archive_Tar', 'Archive/Tar.php', "$dist_dir/PEAR/go-pear-bundle");
	extract_file_from_tarball('Console_Getopt', 'Console/Getopt.php', "$dist_dir/PEAR/go-pear-bundle");
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
				if ($use_pear_template) {
					/* copy to top level */
					copy_dir($item, "$dist_dir/$bi");
				}
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
	
	/* copy c++ runtime */
	$items = glob("$snapshot_template/dlls/*.CRT");

	foreach ($items as $item) {
		$bi = basename($item);
		if (is_dir($item)) {
			copy_dir($item, "$dist_dir/$bi");
			copy_dir($item, "$dist_dir/ext/$bi");
		}
	}
} else {
	echo "WARNING: you don't have a snapshot template\n";
	echo "         your dist will not be complete\n";
}

?>
