<?php

if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}

include dirname(__FILE__)."/include.inc";

if (!get_fpm_path()) {
	die("skip FPM not found");
}

?>
