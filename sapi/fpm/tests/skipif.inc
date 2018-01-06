<?php

if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}


if (!getmyuid() && !getenv('TEST_FPM_RUN_AS_ROOT')) {
	die('Refusing to run as root');
}

include dirname(__FILE__)."/include.inc";

if (!get_fpm_path()) {
	die("skip FPM not found");
}
?>
