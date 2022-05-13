--TEST--
Test SNMP object property errors
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

$session = new SNMP(SNMP::VERSION_1, $hostname, $community, $timeout, $retries);

try {
    $session->info = [];
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->info += [];
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->max_oids = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->max_oids = -1;
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->valueretrieval = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->quick_print = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->enum_print = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->oid_output_format = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->oid_increasing_check = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $session->exceptions_enabled = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot write read-only property SNMP::$info
Cannot write read-only property SNMP::$info
Cannot assign array to property SNMP::$max_oids of type ?int
SNMP::$max_oids must be greater than 0 or null
Cannot assign array to property SNMP::$valueretrieval of type int
Cannot assign array to property SNMP::$quick_print of type bool
Cannot assign array to property SNMP::$enum_print of type bool
Cannot assign array to property SNMP::$oid_output_format of type int
Cannot assign array to property SNMP::$oid_increasing_check of type bool
Cannot assign array to property SNMP::$exceptions_enabled of type int
