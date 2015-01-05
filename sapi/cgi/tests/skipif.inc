<?php

if (substr(php_sapi_name(), 0, 3) == "cgi") {
	exit;
}

if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}

include dirname(__FILE__)."/include.inc";

if (!get_cgi_path()) {
	die("skip CGI not found");
}

?>
