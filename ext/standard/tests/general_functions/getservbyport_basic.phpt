--TEST--
Test function getservbyport() by calling it more than or less than its expected arguments
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--FILE--
<?php
	if(stristr(PHP_OS, "linux")) $file = "/etc/services";
	elseif(stristr(PHP_OS, "Darwin")) $file = "/etc/services";
	elseif(substr(PHP_OS,0,3) == "WIN") $file = "C:/WINDOWS/system32/drivers/etc/services";
	else die(PHP_OS. " unsupported");

	if(file_exists($file)){
		var_dump(getservbyport( 80, "tcp" ) === "www");
	}else{
		var_dump(getservbyport(80, "tcp") === false);	
	}
?>
--EXPECTF--
bool(true)
