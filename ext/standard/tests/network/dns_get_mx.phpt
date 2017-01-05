--TEST--
bool dns_get_mx ( string $hostname , array &$mxhosts [, array &$weight ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS"))
    die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS"))
    die("skip test requiring internet connection");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip: no Windows support');
}
?>
--FILE--
<?php
$domains = array('yahoo.co.jp', 'yahoo.com', 'es.yahoo.com', 'fr.yahoo.com', 'it.yahoo.com');
foreach ($domains as $domain) {
    if (getmxrr($domain, $hosts, $weights)) {
        echo "Hosts: " . count($hosts) . ", weights: " . count($weights) . "\n";
    }
}
?>
--EXPECTF--
Hosts: %i, weights: %i
Hosts: %i, weights: %i
Hosts: %i, weights: %i
Hosts: %i, weights: %i
Hosts: %i, weights: %i
