--TEST--
ldap_control_assertion() - Assertion control creation error
--CREDITS--
Tiziano Müller <tiziano.mueller@stepping-stone.ch>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link);

$entry = array(
	"objectClass"	=> array(
		"top",
		"dcObject",
		"organization"),
	"dc"		=> "my-domain",
	"o"		=> "my-domain",
	"description"	=> "Domain description",
);
ldap_modify($link, "dc=my-domain,dc=com", $entry);

$garbage_assertion_string = "garbage assertion string";
$invalid_assertion_string = "(description=Invalid domain description)";

$session_tracking_control = ldap_control_session_tracking($link, "127.0.0.1", "localhost", LDAP_CONTROL_X_SESSION_TRACKING_USERNAME, "testuser");
$invalid_assertion_control = ldap_control_assertion($link, $invalid_assertion_string);

var_dump(
	ldap_control_assertion($link, $garbage_assertion_string)
);

var_dump(
	$invalid_assertion_control,
	ldap_modify($link, "dc=my-domain,dc=com", $entry, $invalid_assertion_control),
	ldap_modify($link, "dc=my-domain,dc=com", $entry, [$invalid_assertion_control])
);

var_dump(
	ldap_modify($link, "dc=my-domain,dc=com", $entry, [$session_tracking_control, $invalid_assertion_control])
);

?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link);
?>
--EXPECTF--
Warning: ldap_control_assertion(): Unable to create assertion control in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Assertion Failed in %s on line %d

Warning: ldap_modify(): Modify: Assertion Failed in %s on line %d
resource(%d) of type (ldap control)
bool(false)
bool(false)

Warning: ldap_modify(): Modify: Assertion Failed in %s on line %d
bool(false)
===DONE===
