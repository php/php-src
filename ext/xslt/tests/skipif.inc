<?php
if(!extension_loaded("xslt") && ini_get("enable_dl")) {
	$dlext = (substr(PHP_OS, 0, 4) == "WIN") ? ".dll" : ".so";
	@dl("xlst$dlext");
}
if(!extension_loaded("xslt")) {
	die("skip\n");
}
?>
