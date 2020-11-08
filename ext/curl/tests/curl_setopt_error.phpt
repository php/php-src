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

try {
    curl_setopt($ch, -10, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    curl_setopt($ch, 1000, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** curl_setopt() call with incorrect parameters
curl_setopt(): Argument #2 ($option) must be of type int, string given
curl_setopt(): Argument #2 ($option) is not a valid cURL option
curl_setopt(): Argument #2 ($option) is not a valid cURL option
