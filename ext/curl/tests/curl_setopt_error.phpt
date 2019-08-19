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

try {
    curl_setopt($ch, '', false);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

curl_setopt($ch, -10, 0);
?>
--EXPECTF--
*** curl_setopt() call with incorrect parameters
curl_setopt() expects parameter 2 to be int, string given

Warning: curl_setopt(): Invalid curl configuration option in %scurl_setopt_error.php on line %d
