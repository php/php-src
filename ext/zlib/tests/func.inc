<?php

function get_zlib_version()
{
	$version = NULL;

	ob_start();
	phpinfo();
	$info = ob_get_contents();
	ob_end_clean();
	//if (preg_match(',zlib.*Compiled Version => (\d+\.\d+\.\d+),s', $info, $match)) {
	//	$version = $match[1];
	if (preg_match(',zlib(?!.*libXML).*Compiled Version (=> |</).*(\d+\.\d+\.\d+?),sU', $info, $match)) {
		$version = $match[2];
	}

	return $version;
}
