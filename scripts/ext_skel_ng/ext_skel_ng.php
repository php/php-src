<?php
	require_once "extension_parser.php";

	$ext = &new extension_parser(fopen("extension.xml", "r"));

  system("rm -rf {$ext->name}");
  mkdir($ext->name);

	// generate code
	$ext->write_header_file();
	$ext->write_code_file();

	// generate project files for configure and ms dev studio
	$ext->write_config_m4();
	$ext->write_ms_devstudio_dsp();

	$ext->generate_documentation();
?>