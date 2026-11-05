--TEST--
new \LDAP\Connection() - Variation
--EXTENSIONS--
ldap
--FILE--
<?php
$constructorCalls = [
    // no hostname, no port
    [],
    // URI
    ["ldap://hostname:389"],
    // ldaps URI
    ["ldaps://hostname:689"],
    // URI no port
    ["ldap://hostname"],
];

foreach ($constructorCalls as $parameters) {
    $link = new \LDAP\Connection(...$parameters);
    var_dump($link);
    ldap_get_option($link, LDAP_OPT_HOST_NAME, $hostname);
    var_dump($hostname);
}
?>
--EXPECTF--
object(LDAP\Connection)#%d (0) {
}
string(%d) "%s:%d"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:389"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:689"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:389"
