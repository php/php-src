--TEST--
PDO_OCI: connect
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_oci")) print "skip"; 
?>
--FILE--
<?php /* $Id$ */

require "settings.inc";

$db = new PDO("oci:dbname=$dbase",$user,$password) or die("connect error");
echo "done\n";
	
?>
--EXPECT--
done
