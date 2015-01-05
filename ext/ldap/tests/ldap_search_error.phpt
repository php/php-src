--TEST--
ldap_search() - operation that should fail
--CREDITS--
Davide Mendolia <idaf1er@gmail.com>
Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once dirname(__FILE__) .'/skipif.inc'; ?>
<?php require_once dirname(__FILE__) .'/skipifbindfailure.inc'; ?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect($host, $port);

$dn = "dc=my-domain,dc=com";
$filter = "(dc=*)";

$result = ldap_search();
var_dump($result);

$result = ldap_search($link, $dn, $filter);
var_dump($result);

$result = ldap_search($link, $dn, $filter, NULL);
var_dump($result);

$result = ldap_search($link, $dn, $filter, array(1 => 'top'));
var_dump($result);

$result = ldap_search(array(), $dn, $filter, array('top'));
var_dump($result);

$result = ldap_search(array($link, $link), array($dn), $filter, array('top'));
var_dump($result);

$result = ldap_search(array($link, $link), $dn, array($filter), array('top'));
var_dump($result);
?>
===DONE===
--EXPECTF--
Warning: ldap_search() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: ldap_search(): Search: No such object in %s on line %d
bool(false)

Warning: ldap_search() expects parameter 4 to be array, null given in %s on line %d
NULL

Warning: ldap_search(): Array initialization wrong in %s on line %d
bool(false)

Warning: ldap_search(): No links in link array in %s on line %d
bool(false)

Warning: ldap_search(): Base must either be a string, or an array with the same number of elements as the links array in %s on line %d
bool(false)

Warning: ldap_search(): Filter must either be a string, or an array with the same number of elements as the links array in %s on line %d
bool(false)
===DONE===
