--TEST--
ldap_set_option() - Leaks attrvalue and context
--EXTENSIONS--
ldap
zend_test
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($uri);

$attrvalue = zend_return_non_interned_string("attrvalue");
$context = zend_return_non_interned_string("context");

$controls = [
    ["oid" => "2.16.840.1.113730.3.4.9", "value" => ["attrvalue" => $attrvalue, "context" => $context, "before" => 0, "after" => 0]],
];

ldap_set_option($link, LDAP_OPT_CLIENT_CONTROLS, $controls);
ldap_get_option($link, LDAP_OPT_CLIENT_CONTROLS, $controls_out);

var_dump($controls_out);
?>
--EXPECTF--
array(1) {
  ["2.16.840.1.113730.3.4.9"]=>
  array(3) {
    ["oid"]=>
    string(23) "2.16.840.1.113730.3.4.9"
    ["iscritical"]=>
    bool(false)
    ["value"]=>
    string(28) "0%0%0Å	attrvaluecontext"
  }
}
