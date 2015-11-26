--TEST--
getmxrr() test
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip: no Windows support');
}
?>
--FILE--
<?php
$domains = array( 'mx1.tests.php.net', 'mx2.tests.php.net' );
foreach ( $domains as $domain )
{
    if ( getmxrr( $domain, $hosts, $weights ) )
    {
        echo "Hosts: " . count( $hosts ) . ", weights: " . count( $weights ) . "\n";
    }
}
?>
--EXPECT--
Hosts: 1, weights: 1
Hosts: 2, weights: 2
