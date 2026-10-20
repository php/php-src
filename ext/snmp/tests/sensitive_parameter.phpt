--TEST--
Test that community strings and SNMPv3 passphrases are marked sensitive.
--EXTENSIONS--
snmp
--FILE--
<?php
try {
    var_dump(snmpget('127.0.0.1', 'public', new stdClass));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    var_dump(snmp3_get('127.0.0.1', 'user', 'authPriv', 'MD5', 'auth-pass', 'DES', 'priv-pass', new stdClass));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    new SNMP(SNMP::VERSION_1, '127.0.0.1', 'public', 'not-an-int');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
TypeError: snmpget(): Argument #3 ($object_id) must be of type array|string, stdClass given in %s:%d
Stack trace:
#0 %s(%d): snmpget('127.0.0.1', Object(SensitiveParameterValue), Object(stdClass))
#1 {main}
TypeError: snmp3_get(): Argument #8 ($object_id) must be of type array|string, stdClass given in %s:%d
Stack trace:
#0 %s(%d): snmp3_get('127.0.0.1', 'user', 'authPriv', 'MD5', Object(SensitiveParameterValue), 'DES', Object(SensitiveParameterValue), Object(stdClass))
#1 {main}
TypeError: SNMP::__construct(): Argument #4 ($timeout) must be of type int, string given in %s:%d
Stack trace:
#0 %s(%d): SNMP->__construct(0, '127.0.0.1', Object(SensitiveParameterValue), 'not-an-int')
#1 {main}
