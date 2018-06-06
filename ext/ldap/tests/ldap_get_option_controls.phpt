--TEST--
ldap_get_option() and ldap_set_option() tests related to ldap controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

function build_ctrl_paged_value($int, $cookie)
{
	// This is basic and will only work for small values
	$hex = '';
	if (!empty($int)) {
		$str = sprintf("%'.02x", $int);
		$hex .= '02'.sprintf("%'.02x%s", strlen($str)/2, $str);
	}
	$hex .= '04'.sprintf("%'.02x", strlen($cookie)).bin2hex($cookie);
	return hex2bin('30'.sprintf("%'.02x", strlen($hex)/2).$hex);
}

$controls_set = array(
	array(
		'oid' => LDAP_CONTROL_PAGEDRESULTS,
		'iscritical' => TRUE,
		'value' => build_ctrl_paged_value(1, 'opaque')
	)
);
$controls_set2 = array(
	array(
		'oid' => LDAP_CONTROL_PAGEDRESULTS,
		'iscritical' => TRUE,
		'value' => array(
			'size' => 1,
			'cookie' => '',
		)
	)
);
var_dump(
	bin2hex($controls_set[0]['value']),
	ldap_get_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_get),
	ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_set),
	ldap_get_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_get),
	$controls_get,
	ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_set2),
	ldap_get_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_get),
	$controls_get,
	$result = ldap_search($link, $base, "(objectClass=person)", array('cn')),
	ldap_get_entries($link, $result)['count'],
	ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, array()),
	ldap_get_option($link, LDAP_OPT_SERVER_CONTROLS, $controls_get)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
string(26) "300b02010104066f7061717565"
bool(false)
bool(true)
bool(true)
array(1) {
  ["1.2.840.113556.1.4.319"]=>
  array(3) {
    ["oid"]=>
    string(22) "1.2.840.113556.1.4.319"
    ["iscritical"]=>
    bool(true)
    ["value"]=>
    array(2) {
      ["size"]=>
      int(1)
      ["cookie"]=>
      string(6) "opaque"
    }
  }
}
bool(true)
bool(true)
array(1) {
  ["1.2.840.113556.1.4.319"]=>
  array(3) {
    ["oid"]=>
    string(22) "1.2.840.113556.1.4.319"
    ["iscritical"]=>
    bool(true)
    ["value"]=>
    array(2) {
      ["size"]=>
      int(1)
      ["cookie"]=>
      string(0) ""
    }
  }
}
resource(%d) of type (ldap result)
int(1)
bool(true)
bool(false)
===DONE===
