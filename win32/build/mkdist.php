<?php
/* piece together a windows binary distro */

$php_version = $argv[1];
$build_dir = $argv[2];
$php_build_dir = $argv[3];
$phpdll = $argv[4];
$sapi_targets = explode(" ", $argv[5]);
$ext_targets = explode(" ", $argv[6]);
$pecl_targets = explode(" ", $argv[7]);
$snapshot_template = $argv[8];

$is_debug = preg_match("/^debug/i", $build_dir);

echo "Making dist for $build_dir\n";

$dist_dir = $build_dir . "/php-" . $php_version;
$test_dir = $build_dir . "/php-test-pack-" . $php_version;
$pecl_dir = $build_dir . "/pecl-" . $php_version;

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
        'secur32.dll', 'netapi32.dll', 'dnsapi.dll', 'psapi.dll', 'normaliz.dll',
        'iphlpapi.dll', 'bcrypt.dll',

        /* apache */
        'apachecore.dll',

        /* apache 2 */
        'libhttpd.dll', 'libapr.dll', 'libaprutil.dll','libapr-1.dll', 'libaprutil-1.dll',

        /* oracle */
        'oci.dll', 'ociw32.dll',

        /* sybase */
        'libcs.dll', 'libct.dll',

        /* firebird */
        'fbclient.dll',

        /* visual C++; mscvrt.dll is present on everyones system,
         * but the debug version (msvcrtd.dll) and those from visual studio.net
         * (msvcrt7x.dll) are not */
        'msvcrt.dll',
        'msvcr90.dll',
        'wldap32.dll',
        'vcruntime140.dll',
        'msvcp140.dll',
        );
    static $no_dist_re = array(
        "api-ms-win-crt-.+\.dll",
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
        } else {
            $skip = false;
            foreach ($no_dist_re as $re) {
                if (preg_match(",$re,", basename($dep)) > 0) {
                    $skip = true;
                    break;
                }
            }
            if ($skip) {
                continue;
            }
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

        if (!isset($per_module_deps[basename($module)]) || !in_array($dep, $per_module_deps[basename($module)])) {
            $per_module_deps[basename($module)][] = $dep;
            //recursively check dll dependencies
            get_depends($dep);
        }
    }
    fclose($pipes[1]);
    proc_close($proc);
//echo "Module $module [$n lines]\n";
}

function copy_file_list($source_dir, $dest_dir, $list)
{
    global $is_debug, $dist_dir;

    foreach ($list as $item) {
        if (empty($item)) {
            continue;
        } elseif (!is_file($source_dir . DIRECTORY_SEPARATOR . $item)) {
            echo "WARNING: $item not found\n";
            continue;
        }

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
            $checksum += ord($hdr_data[$i]);
        for ($i = 148; $i < 156; $i++)
            $checksum += 32;
        for ($i = 156; $i < 512; $i++)
            $checksum += ord($hdr_data[$i]);

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
/* copy dll and its dependencies */
copy_file_list($build_dir, "$dist_dir", [$phpdll]);

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
if(sizeof($pecl_targets)) {
    copy_file_list($build_dir, $pecl_dir, $pecl_targets);
}

/* populate reading material */
$text_files = array(
    "LICENSE" => "license.txt",
    "NEWS" => "news.txt",
    "README.md" => "README.md",
    "README.REDIST.BINS" => "readme-redist-bins.txt",
    "php.ini-development" => "php.ini-development",
    "php.ini-production" => "php.ini-production"
);

foreach ($text_files as $src => $dest) {
    copy_text_file($src, $dist_dir . '/' . $dest);
}

/* general other files */
$general_files = array(
    "$GLOBALS[build_dir]\\deplister.exe"	=>	"deplister.exe",
);

foreach ($general_files as $src => $dest) {
    copy($src, $dist_dir . '/' . $dest);
}

/* include a snapshot identifier */
$branch = "HEAD"; // TODO - determine this from GitHub branch name
$fp = fopen("$dist_dir/snapshot.txt", "w");
$now = date("r");
fwrite($fp, <<<EOT
This snapshot was automatically generated on
$now

Version: $php_version
Branch: $branch
Build: $build_dir

EOT
);
/* list built-in extensions */
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
            $tdll = $php_build_dir . '/bin/' . basename($dll);
            if (!file_exists($tdll)) {
                echo "WARNING: distro depends on $dll, but could not find it on your system\n";
                continue;
            }
        }
        $dll = $tdll;
    }
    copy($dll, "$dist_dir/" . basename($dll));
}

/* TODO:
add sanity check and test if all required DLLs are present, per version
This version works at least for 3.6, 3.8 and 4.0 (5.3-vc6, 5.3-vc9 and HEAD).
Add ADD_DLLS to add extra DLLs like dynamic dependencies for standard
deps. For example, libenchant.dll loads libenchant_myspell.dll or
libenchant_ispell.dll
*/
$ENCHANT_DLLS = array(
    array('', 'glib-2.dll'),
    array('', 'gmodule-2.dll'),
);
if (file_exists("$php_build_dir/bin/libenchant2.dll")) {
    $ENCHANT_DLLS[] = array('lib/enchant', 'libenchant2_hunspell.dll');
} else {
    $ENCHANT_DLLS[] = array('lib/enchant', 'libenchant_myspell.dll');
    $ENCHANT_DLLS[] = array('lib/enchant', 'libenchant_ispell.dll');
}
foreach ($ENCHANT_DLLS as $dll) {
    $dest  = "$dist_dir/$dll[0]";
    $filename = $dll[1];

    if (!file_exists("$dest") || !is_dir("$dest")) {
        if (!mkdir("$dest", 0777, true)) {
            echo "WARNING: couldn't create '$dest' for enchant plugins ";
        }
    }

    if (!copy($php_build_dir . '/bin/' . $filename, "$dest/" . basename($filename))) {
            echo "WARNING: couldn't copy $filename into the dist dir";
    }
}

$OPENSSL_DLLS = $php_build_dir . "/lib/ossl-modules/*.dll";
$fls = glob($OPENSSL_DLLS);
if (!empty($fls)) {
    $openssl_dest_dir = "$dist_dir/extras/ssl";
    if (!file_exists($openssl_dest_dir) || !is_dir($openssl_dest_dir)) {
        if (!mkdir($openssl_dest_dir, 0777, true)) {
            echo "WARNING: couldn't create '$openssl_dest_dir' for OpenSSL providers ";
        }
    }
    foreach ($fls as $fl) {
        if (!copy($fl, "$openssl_dest_dir/" . basename($fl))) {
            echo "WARNING: couldn't copy $fl into the $openssl_dest_dir";
        }
    }
}

$SASL_DLLS = $php_build_dir . "/bin/sasl2/sasl*.dll";
$fls = glob($SASL_DLLS);
if (!empty($fls)) {
    $sasl_dest_dir = "$dist_dir/sasl2";
    if (!file_exists($sasl_dest_dir) || !is_dir($sasl_dest_dir)) {
        if (!mkdir("$sasl_dest_dir", 0777, true)) {
            echo "WARNING: couldn't create '$sasl_dest_dir' for SASL2 auth plugins ";
        }
    }
    foreach ($fls as $fl) {
        if (!copy($fl, "$sasl_dest_dir/" . basename($fl))) {
            echo "WARNING: couldn't copy $fl into the $sasl_dest_dir";
        }
    }
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
        if ($f == '.' || $f == '..' || $f == '.svn') {
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



function copy_test_dir($directory, $dest)
{
    if(substr($directory,-1) == '/') {
        $directory = substr($directory,0,-1);
    }

    if ($directory == 'tests' || $directory == 'examples') {
        if (!is_dir($dest . '/tests')) {
            mkdir($dest . '/tests', 0775, true);
        }
        copy_dir($directory, $dest . '/tests/');

        return false;
    }

    if(!file_exists($directory) || !is_dir($directory)) {
        echo "failed... $directory\n";
        return FALSE;
    }

    $directory_list = opendir($directory);

    while (FALSE !== ($file = readdir($directory_list))) {
        $full_path = $directory . '/' . $file;
        if($file != '.' && $file != '..' && $file != '.svn' && is_dir($full_path)) {
            if ($file == 'tests' || $file == 'examples') {
                if (!is_dir($dest . '/' . $full_path)) {
                    mkdir($dest . '/' . $full_path , 0775, true);
                }
                copy_dir($full_path, $dest . '/' . $full_path . '/');
                continue;
            } else {
                copy_test_dir($full_path, $dest);
            }
        }
    }

    closedir($directory_list);
}

function make_phar_dot_phar($dist_dir)
{
    if (!extension_loaded('phar')) {
        return;
    }

    $path_to_phar = realpath(__DIR__ . '/../../ext/phar');

    echo "Generating pharcommand.phar\n";
    $phar = new Phar($dist_dir . '/pharcommand.phar', 0, 'pharcommand');

    foreach (new DirectoryIterator($path_to_phar . '/phar') as $file) {
        if ($file->isDir() || $file == 'phar.php') {
            continue;
        }

        echo 'adding ', $file, "\n";
        $phar[(string) $file] = file_get_contents($path_to_phar.  '/phar/' . $file);
    }

    $phar->setSignatureAlgorithm(Phar::SHA1);
    $stub = file($path_to_phar . '/phar/phar.php');

    unset($stub[0]); // remove hashbang
    $phar->setStub(implode('', $stub));

    echo "Creating phar.phar.bat\n";
    file_put_contents($dist_dir . '/phar.phar.bat', "\"%~dp0php.exe\" \"%~dp0pharcommand.phar\" %*\r\n");
}

if (!is_dir($test_dir)) {
    mkdir($test_dir);
}

$dirs = array(
    'ext',
    'Sapi',
    'Zend',
    'tests'
);
foreach ($dirs as $dir) {
    copy_test_dir($dir, $test_dir);
}
copy('run-tests.php', $test_dir . '/run-tests.php');

/* change this next line to true to use good-old
 * hand-assembled go-pear-bundle from the snapshot template */
$use_pear_template = true;

if (!$use_pear_template) {
    /* Let's do a PEAR-less pear setup */
    mkdir("$dist_dir/PEAR");
    mkdir("$dist_dir/PEAR/go-pear-bundle");

    /* grab the bootstrap script */
    echo "Downloading go-pear\n";
    copy("https://pear.php.net/go-pear.phar", "$dist_dir/PEAR/go-pear.php");

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
    echo "WARNING: you don't have a snapshot template, your dist will not be complete\n";
}

make_phar_dot_phar($dist_dir);
?>
