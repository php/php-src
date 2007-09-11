<?php

// script to upgrade PCRE. just drop the pcre-x.x.tar.xx here and run the script

$pattern = 'pcre-*.tar.*';
$newpcre = glob($pattern);

if (count($newpcre) > 1) {
	echo "more than one '$pattern' file. aborting\n";
	print_r($newpcre);
	exit;
}

if (count($newpcre) == 0) {
	die("need one '$pattern' file. aborting.\n");
}


$newpcre = $newpcre[0];

if (strpos($newpcre, 'gz')) {
	system("tar xfz $newpcre");
} elseif (strpos($newpcre, 'bz2')) {
	system("tar xfj $newpcre");
} else {
	die("file type not recognized: $newpcre\n");
}

$newpcre = substr($newpcre, 0, strpos($newpcre, '.tar'));
$dirlen = strlen('pcrelib');

function recurse($path)
{
	global $newpcre, $dirlen;

	foreach (scandir($path) as $file) {

		if ($file[0] === '.' ||
		$file === 'CVS' ||
		substr_compare($file, '.lo', -3, 3) == 0 ||
		substr_compare($file, '.loT', -4, 4) == 0 ||
		substr_compare($file, '.o', -2, 2) == 0) continue;

		$file = "$path/$file";

		if (is_dir($file)) {
			recurse($file);
			continue;
		}

		echo "processing $file... ";

		$newfile = $newpcre . substr($file, $dirlen);

		if (is_file($tmp = $newfile . '.generic') || is_file($tmp = $newfile . '.dist')) {
			$newfile = $tmp;
		}


		if (!is_file($newfile)) {
			die("$newfile is not available any more\n");
		}

		// maintain file mtimes so that cvs doesnt get crazy
		if (file_get_contents($newfile) !== file_get_contents($file)) {
			copy($newfile, $file);
		}

		// always include the config.h file
		$content    = file_get_contents($newfile);
		$newcontent = preg_replace('/#\s*ifdef HAVE_CONFIG_H\s*(.+)\s*#\s*endif/', '$1', $content);

		if ($content !== $newcontent) {
			file_put_contents($file, $newcontent);
		}

		echo "OK\n";
	}

}


recurse('pcrelib');

$dirorig = scandir('pcrelib/testdata');
$k = array_search('CVS', $dirorig);
unset($dirorig[$k]);

$dirnew = scandir("$newpcre/testdata");
$diff   = array_diff($dirorig, $dirnew);

foreach ($diff as $file) {
	$file2 = 'pcrelib'.substr($file, strlen($newpcre));
	copy($file, $file2);
}


// the config.h needs special care
$prepend_config_h = '
#include <php_compat.h>
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME
#undef PACKAGE_STRING

#define SUPPORT_UCP
#define SUPPORT_UTF8


';

file_put_contents('pcrelib/config.h', $prepend_config_h . file_get_contents('pcrelib/config.h'));


echo "\nThe End :-)\n\n"

?>
