--TEST--
dns_get_record() CAA tests
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php
/* This must be domains which publish an RFC6844 CAA-type DNS record */
$domains = ["big.basic.caatestsuite.com", "google.com"];
foreach ($domains as $domain) {
    $match = false;
    $dns = dns_get_record($domain, DNS_CAA);
    if (count($dns) > 0) {
        if (array_key_exists("type", $dns[0])
        and $dns[0]["type"] == "CAA"
        and array_key_exists("flags", $dns[0])
        and array_key_exists("tag", $dns[0])
        and array_key_exists("value", $dns[0])
        ) {
            $chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-.";
            if (strlen($dns[0]["value"]) == strspn($dns[0]["value"], $chars)) {
                $match = true;
            }
        }
    }
    if ($match) {
        echo "CAA record found\n";
    } else {
        echo "CAA lookup failed\n";
        var_dump($dns);
    }
}
?>
--EXPECT--
CAA record found
CAA record found
