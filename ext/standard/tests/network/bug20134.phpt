--TEST--
Bug #20134 (UDP reads from invalid ports)
--FILE--
<?php
# vim600:syn=php:
$fp = fsockopen("udp://localhost", 65534, $errno, $errstr);
if (!$fp) {
	/* UDP will never cause a connection error, as it is
	 * a connection-LESS protocol */
    echo "ERROR: $errno - $errstr<br>\n";
}
else {
	/* Likewise, writes will always appear to succeed */
    $x = fwrite($fp,"\n");
	var_dump($x);
	/* But reads should always fail */
    $content = fread($fp, 40);
	var_dump($content);
    fclose($fp);
}
?>
--EXPECT--
int(1)
string(0) ""
