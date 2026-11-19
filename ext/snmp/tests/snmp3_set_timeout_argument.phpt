--TEST--
snmp3_set() - an invalid timeout or retries cites the right argument
--EXTENSIONS--
snmp
--FILE--
<?php
$oid = '.1.3.6.1.2.1.1.5.0';
foreach ([[-2, 1], [100, -2]] as [$timeout, $retries]) {
    try {
        snmp3_set('127.0.0.1', 'user', 'noAuthNoPriv', '', '', '', '', $oid, 's', 'x', $timeout, $retries);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
ValueError: snmp3_set(): Argument #11 ($timeout) must be between -1 and %d
ValueError: snmp3_set(): Argument #12 ($retries) must be between -1 and %d
