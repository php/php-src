--TEST--
GHSA-g665-fm4p-vhff (OOB access in ldap_escape)
--EXTENSIONS--
ldap
--INI--
memory_limit=-1
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 4) die("skip only for 32-bit");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
try {
    ldap_escape(str_repeat("*", 1431655759), "", LDAP_ESCAPE_FILTER);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// would allocate a string of length 2
try {
    ldap_escape(str_repeat("*", 1431655766), "", LDAP_ESCAPE_FILTER);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ldap_escape(): Argument #1 ($value) is too long
ldap_escape(): Argument #1 ($value) is too long
