--TEST--
getmxrr() test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip: no Windows support');
}
?>
--FILE--
<?php
$domains = array( 'php.net', 'lists.php.net' );
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
