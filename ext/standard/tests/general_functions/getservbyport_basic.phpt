--TEST--
Test function getservbyport() by calling it more than or less than its expected arguments
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--SKIPIF--
<?php
    if(in_array(PHP_OS_FAMILY, ['BSD', 'Darwin', 'Solaris', 'Linux'])){
        if (!file_exists("/etc/services")) die("skip reason: missing /etc/services");
    }
?>
--FILE--
<?php
	if (file_exists("/etc/services")) {
		$file = "/etc/services";
	}
	elseif(substr(PHP_OS,0,3) == "WIN") $file = "C:/WINDOWS/system32/drivers/etc/services";
	else die(PHP_OS. " unsupported");

	if(file_exists($file)){
		$services = file_get_contents($file);
                $service = getservbyport( 80, "tcp" );
                if(preg_match("/$service\s+80\/tcp/", $services)) {
			echo "PASS\n";
		}
	}else{
		echo "Services file not found in expected location\n";
	}
?>
--EXPECT--
PASS
