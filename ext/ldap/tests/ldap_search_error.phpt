--TEST--
ldap_search() - operation that should fail
--CREDITS--
Davide Mendolia <idaf1er@gmail.com>
Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once __DIR__ .'/skipifbindfailure.inc'; ?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect($uri);

$dn = "dc=not-found,$base";
$filter = "(dc=*)";

$result = ldap_search($link, $dn, $filter);
var_dump($result);

try {
    $result = ldap_search($link, $dn, $filter, array('foo' => 'top'));
    var_dump($result);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ldap_search(array(), $dn, $filter, array('top'));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ldap_search(array($link, $link), array($dn), $filter, array('top'));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ldap_search(array($link, $link), $dn, array($filter), array('top'));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ldap_search($link, [], []);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ldap_search($link, "", []);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
Warning: ldap_search(): Search: No such object in %s on line %d
bool(false)
ldap_search(): Argument #4 ($attributes) must be an array with numeric keys
ldap_search(): Argument #1 ($ldap) must not be empty
ldap_search(): Argument #2 ($base) must be the same size as argument #1
ldap_search(): Argument #3 ($filter) must be the same size as argument #1
ldap_search(): Argument #2 ($base) must be of type string when argument #1 ($ldap) is an LDAP\Connection instance
ldap_search(): Argument #3 ($filter) must be of type string when argument #1 ($ldap) is an LDAP\Connection instance
