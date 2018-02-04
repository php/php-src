--TEST--
curl_setopt() basic parameter test
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php
echo "*** curl_setopt() call with incorrect parameters\n";
$ch = curl_init();
curl_setopt();
curl_setopt(false);

curl_setopt($ch);
curl_setopt($ch, false);
curl_setopt($ch, -10);
curl_setopt($ch, '');
curl_setopt($ch, 1, false);

curl_setopt(false, false, false);
curl_setopt($ch, '', false);
curl_setopt($ch, 1, '');
curl_setopt($ch, -10, 0);
?>
--EXPECTF--
*** curl_setopt() call with incorrect parameters

Warning: curl_setopt() expects exactly 3 parameters, 0 given in %s on line %d

Warning: curl_setopt() expects exactly 3 parameters, 1 given in %s on line %d

Warning: curl_setopt() expects exactly 3 parameters, 1 given in %s on line %d

Warning: curl_setopt() expects exactly 3 parameters, 2 given in %s on line %d

Warning: curl_setopt() expects exactly 3 parameters, 2 given in %s on line %d

Warning: curl_setopt() expects exactly 3 parameters, 2 given in %s on line %d

Warning: curl_setopt() expects parameter 1 to be resource, bool given in %s on line %d

Warning: curl_setopt() expects parameter 2 to be int, string given in %s on line %d

Warning: curl_setopt(): Invalid curl configuration option in %scurl_setopt_error.php on line %d
