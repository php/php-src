--TEST--
ldap_add() - Add operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

// Too few parameters
var_dump(ldap_add());
var_dump(ldap_add($link));
var_dump(ldap_add($link, "dc=my-domain,dc=com"));

// Too many parameters
var_dump(ldap_add($link, "dc=my-domain,dc=com", array(), "Additional data"));

var_dump(ldap_add($link, "dc=my-domain,dc=com", array()));

// Invalid DN
var_dump(
	ldap_add($link, "weirdAttribute=val", array(
		"weirdAttribute"			=> "val",
	)),
	ldap_error($link),
	ldap_errno($link)
);

// Duplicate entry
for ($i = 0; $i < 2; $i++)
	var_dump(
		ldap_add($link, "dc=my-domain,dc=com", array(
			"objectClass"	=> array(
				"top",
				"dcObject",
				"organization"),
			"dc"			=> "my-domain",
			"o"				=> "my-domain",
		))
	);
var_dump(ldap_error($link), ldap_errno($link));

// Wrong array indexes
var_dump(
	ldap_add($link, "dc=my-domain2,dc=com", array(
		"objectClass"	=> array(
			0	=> "top",
			2	=> "dcObject",
			5	=> "organization"),
		"dc"			=> "my-domain",
		"o"				=> "my-domain",
	))
	/* Is this correct behaviour to still have "Already exists" as error/errno?
	,
	ldap_error($link),
	ldap_errno($link)
	*/
);

// Invalid attribute
var_dump(
	ldap_add($link, "dc=my-domain,dc=com", array(
		"objectClass"	=> array(
			"top",
			"dcObject",
			"organization"),
		"dc"			=> "my-domain",
		"o"				=> "my-domain",
		"weirdAttr"		=> "weirdVal",
	)),
	ldap_error($link),
	ldap_errno($link)
);

var_dump(
	ldap_add($link, "dc=my-domain,dc=com", array(array( "Oops"
	)))
	/* Is this correct behaviour to still have "Undefined attribute type" as error/errno?
	,
	ldap_error($link),
	ldap_errno($link)
	*/
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,dc=com");
?>
--EXPECTF--
Warning: ldap_add() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: ldap_add() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: ldap_add() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: ldap_add() expects exactly 3 parameters, 4 given in %s on line %d
NULL

Warning: ldap_add(): Add: Protocol error in %s on line %d
bool(false)

Warning: ldap_add(): Add: Invalid DN syntax in %s on line %d
bool(false)
string(17) "Invalid DN syntax"
int(34)
bool(true)

Warning: ldap_add(): Add: Already exists in %s on line %d
bool(false)
string(14) "Already exists"
int(68)

Warning: ldap_add(): Value array must have consecutive indices 0, 1, ... in %s on line %d
bool(false)

Warning: ldap_add(): Add: Undefined attribute type in %s on line %d
bool(false)
string(24) "Undefined attribute type"
int(17)

Warning: ldap_add(): Unknown attribute in the data in %s on line %d
bool(false)
===DONE===
