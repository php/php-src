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
curl_setopt($ch, -1);
curl_setopt($ch, '');
curl_setopt($ch, 1, false);

curl_setopt(false, false, false);
curl_setopt($ch, '', false);
curl_setopt($ch, 1, '');
curl_setopt($ch, -1, 0);
?>
--EXPECTF--
*** curl_setopt() call with incorrect parameters

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: Wrong parameter count for curl_setopt() in %s on line %d

Warning: curl_setopt(): supplied argument is not a valid cURL handle resource in %s on line %d
