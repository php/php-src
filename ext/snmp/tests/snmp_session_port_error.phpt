--TEST--
SNMP rejects malformed remote ports
--EXTENSIONS--
snmp
--FILE--
<?php

$hostnames = [
	'127.0.0.1:',
	'127.0.0.1:not-a-port',
	'127.0.0.1:123junk',
	'127.0.0.1:-1',
	'127.0.0.1:65536',
	'[::1]:',
	'[::1]:123junk',
];

foreach ($hostnames as $hostname) {
	try {
		new SNMP(SNMP::VERSION_1, $hostname, 'public');
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

foreach (['127.0.0.1:0', '127.0.0.1:65535'] as $hostname) {
	var_dump(new SNMP(SNMP::VERSION_1, $hostname, 'public') instanceof SNMP);
}

try {
	snmpget('127.0.0.1:not-a-port', 'public', '1.3.6.1.2.1.1.1.0');
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
ValueError: SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
bool(true)
bool(true)
ValueError: snmpget(): Argument #1 ($hostname) remote port must be between 0 and 65535
