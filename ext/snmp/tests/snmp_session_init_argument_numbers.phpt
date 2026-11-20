--TEST--
Errors of the hostname and the community name the right argument
--EXTENSIONS--
snmp
--FILE--
<?php
$oid = '.1.3.6.1.2.1.1.5.0';
$calls = [
    fn() => snmpget("a\0b", 'public', $oid),
    fn() => snmpget(str_repeat('a', 300), 'public', $oid),
    fn() => snmpget('127.0.0.1:99999', 'public', $oid),
    fn() => snmpget('127.0.0.1', "a\0b", $oid),
    fn() => snmpget('127.0.0.1', '', $oid),
    fn() => snmp3_get("a\0b", 'user', 'noAuthNoPriv', '', '', '', '', $oid),
    fn() => snmp3_get('127.0.0.1', '', 'noAuthNoPriv', '', '', '', '', $oid),
    fn() => new SNMP(SNMP::VERSION_2C, "a\0b", 'public'),
    fn() => new SNMP(SNMP::VERSION_2C, '127.0.0.1', ''),
];
foreach ($calls as $call) {
    try {
        $call();
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
snmpget(): Argument #1 ($hostname) must not contain any null bytes
snmpget(): Argument #1 ($hostname) length must be lower than 128
snmpget(): Argument #1 ($hostname) remote port must be between 0 and 65535
snmpget(): Argument #2 ($community) must not contain any null bytes
snmpget(): Argument #2 ($community) must not be empty
snmp3_get(): Argument #1 ($hostname) must not contain any null bytes
snmp3_get(): Argument #2 ($security_name) must not be empty
SNMP::__construct(): Argument #2 ($hostname) must not contain any null bytes
SNMP::__construct(): Argument #3 ($community) must not be empty
