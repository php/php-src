--TEST--
dns_check_record() segfault with empty host
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip No windows support');
}
?>
--FILE--
<?php
$domains = array( 'php.net', 'ez.no' );
foreach ( $domains as $domain )
{
    if ( getmxrr( $domain, $hosts, $weights ) )
    {
        echo "Hosts: " . count( $hosts ) . ", weights: " . count( $weights ) . "\n";
    }
}
?>
--EXPECT--
Hosts: 2, weights: 2
Hosts: 1, weights: 1
