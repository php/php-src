--TEST--
ldap_add() - Add operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

var_dump(ldap_add($link, "$base", array()));

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
    ldap_add($link, "dc=my-domain,$base", array(
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
try {
    ldap_add($link, "dc=my-domain2,dc=com", array(
        "objectClass"	=> array(
            0	=> "top",
            2	=> "dcObject",
            5	=> "organization"),
        "dc"			=> "my-domain",
        "o"				=> "my-domain",
    ));
    /* Is this correct behaviour to still have "Already exists" as error/errno?
    ,
    ldap_error($link),
    ldap_errno($link)
    */
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

// Invalid attribute
var_dump(
    ldap_add($link, "$base", array(
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
    ldap_add($link, "$base", array(array( "Oops"
    )))
    /* Is this correct behaviour to still have "Undefined attribute type" as error/errno?
    ,
    ldap_error($link),
    ldap_errno($link)
    */
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
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
ldap_add(): Argument #3 ($entry) must contain arrays with consecutive integer indices starting from 0

Warning: ldap_add(): Add: Undefined attribute type in %s on line %d
bool(false)
string(24) "Undefined attribute type"
int(17)

Warning: ldap_add(): Unknown attribute in the data in %s on line %d
bool(false)
