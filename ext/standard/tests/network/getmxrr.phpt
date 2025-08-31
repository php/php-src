--TEST--
getmxrr() test
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip: no Windows support');
}
if (getenv('SKIP_MSAN')) die('skip msan missing interceptor for dn_expand()');
?>
--FILE--
<?php
$domains = array(
    'mx1.tests.php.net.',
    'mx2.tests.php.net.',
    'qa.php.net.',
);
foreach ($domains as $domain) {
    $result = getmxrr($domain, $hosts, $weights);
    echo "Result: " . ($result ? "true" : "false")
       . ", hosts: " . count( $hosts )
       . ", weights: " . count( $weights ) . "\n";
}
?>
--EXPECT--
Result: true, hosts: 1, weights: 1
Result: true, hosts: 2, weights: 2
Result: false, hosts: 0, weights: 0
