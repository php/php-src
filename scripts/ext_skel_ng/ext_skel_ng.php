<?php
	require_once "extension_parser.php";

	$ext = &new extension_parser(fopen("extension.xml", "r"));

  system("rm -rf {$ext->name}");
  mkdir($ext->name);

	$ext->write_config_m4();
	$ext->write_header_file();
	$ext->write_code_file();
	$ext->generate_documentation();
?>