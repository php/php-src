<?php
	$min_version = "5.0.0-dev";
	if (!function_exists("version_compare") || version_compare(phpversion(), $min_version) <0) {
		die("need at least PHP version $min_version, you are running ".phpversion());
	}

  require_once "extension_parser.php";
	require_once "System.php";

	// parse command line arguments
	if (isset($_SERVER['argv'][1])) {
		$filename = $_SERVER["argv"][1];
		echo "using '$filename' as specification file\n";
	} else {
		$filename = "extension.xml";
		echo "using default file '$filename' as specification file\n";
	}

	// open specification file
	$fp = fopen($filename, "r");
	if (!is_resource($fp)) {
		error_log("can't read specification file '$filename'");
		exit;
	}

	// parse specification file
  $ext = new extension_parser($fp);
	fclose($fp);

	// purge and create extension directory
  System::rm("-rf {$ext->name}");
  mkdir($ext->name);

	// write LICENSE file
	if(is_object($ext->license)) {
		$ext->license->write_license_file("{$ext->name}/LICENSE");
	}

  // generate code
  $ext->write_header_file();
  $ext->write_code_file();
  if (isset($ext->logo)) {
    $fp = fopen("{$ext->name}/{$ext->name}_logo.h", "w");
    fwrite($fp, $ext->logo->h_code());
    fclose($fp);
  }

  // generate project files for configure (unices and similar platforms like cygwin)
	if ($ext->platform === "all" || in_array("unix", $ext->platform)) {
    $ext->write_config_m4();
	}

  // generate project files for Windows platform (VisualStudio/C++ V6)
	if ($ext->platform === "all" || in_array("win32", $ext->platform)) {
    $ext->write_ms_devstudio_dsp();
	}

	// generate EXPERIMENTAL file for unstable release states
	$ext->write_experimental();

  // generate CREDITS file
	$ext->write_credits();

	// generate PEAR/PECL package.xml file
	$ext->write_package_xml();

	// generate DocBook XML documantation for PHP manual
  $ext->generate_documentation();

	// generate test case templates
	$ext->write_test_files();

?>
