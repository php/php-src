<?php

if (substr(php_sapi_name(), 0, 3) == "cgi") {
    exit;
}

include dirname(__FILE__)."/include.inc";

if (!get_cgi_path()) {
    die("skip CGI not found");
}

?>
