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

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

function dumpResult(LDAP\Connection $link, LDAP\Result $result): void {
    var_dump(
        $result,
        ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
        $errmsg,
        $errcode,
    );
}

// Invalid DN
var_dump(
    ldap_add(
        $link,
        "weirdAttribute=val",
        ["weirdAttribute" => "val"],
    ),
    ldap_error($link),
    ldap_errno($link)
);

// Invalid DN
dumpResult(
    $link,
    $link->add(
        "weirdAttribute=val",
        ["weirdAttribute" => "val"],
    ),
);

// Duplicate entry
for ($i = 0; $i < 2; $i++) {
    var_dump(
        ldap_add(
            $link,
            "dc=my-domain,$base",
            [
                "objectClass" => [
                    "top",
                    "dcObject",
                    "organization",
                ],
                "dc" => "my-domain",
                "o"	 => "my-domain",
            ],
        )
    );
}
var_dump(ldap_error($link), ldap_errno($link));

// Invalid attribute
var_dump(
    ldap_add(
        $link,
        "dc=my-domain,$base",
        [
            "objectClass" => [
                "top",
                "dcObject",
                "organization",
            ],
            "dc" => "my-domain",
            "o"	 => "my-domain",
            "weirdAttr"	=> "weirdVal",
        ],
    ),
    ldap_error($link),
    ldap_errno($link),
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
Warning: ldap_add(): Add: Invalid DN syntax in %s on line %d
bool(false)
string(17) "Invalid DN syntax"
int(34)
object(LDAP\Result)#%d (0) {
}
bool(true)
string(10) "invalid DN"
int(34)
bool(true)

Warning: ldap_add(): Add: Already exists in %s on line %d
bool(false)
string(14) "Already exists"
int(68)

Warning: ldap_add(): Add: Undefined attribute type in %s on line %d
bool(false)
string(24) "Undefined attribute type"
int(17)
