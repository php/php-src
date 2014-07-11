<?php

function get_gd_version()
{
	return GD_VERSION;
}

function get_php_info()
{
	ob_start();
	phpinfo();
	$info = ob_get_contents();
	ob_end_clean();

	return $info;
}

function get_freetype_version()
{
	$version = 0;
	
	if (preg_match(',FreeType Version => (\d+\.\d+\.\d+),s', get_php_info(), $match)) {
		$version = $match[1];
	}
	
	return $version;
}

function get_libjpeg_version()
{
	$version = 0;
	
	if (preg_match(',libJPEG Version => ([a-z0-9]+),s', get_php_info(), $match)) {
		$version = $match[1];
	}
	
	return $version;
}

function get_libpng_version()
{
	$version = 0;
	
	if (preg_match(',libPNG Version => (\d+\.\d+\.\d+),s', get_php_info(), $match)) {
		$version = $match[1];
	}
	
	return $version;
}

function get_libxpm_version()
{
	$version = 0;
	
	if (preg_match(',libXpm Version => (\d+),s', get_php_info(), $match)) {
		$version = $match[1];
	}
	
	return $version;
}

