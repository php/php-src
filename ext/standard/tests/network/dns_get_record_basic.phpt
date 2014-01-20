--TEST--
dns_get_record() tests
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php
/* This must be a domain that publishes an RFC4408 SPF-type DNS record, */
/* not to be confused with a domain that publishes an SPF record in a TXT record type */
$domain = 'github.com';
$match = false;
$dns = dns_get_record( $domain, DNS_SPF );
if (count($dns) > 0) {
    if (array_key_exists('type', $dns[0]) and $dns[0]['type'] == 'SPF' and
        array_key_exists('spf', $dns[0]) and !empty($dns[0]['spf'])) {
        $match = true;
    }
}
if ($match) {
    echo "SPF Match\n";
} else {
    echo "SPF Lookup failed\n";
}
?>
--EXPECT--
SPF Match
