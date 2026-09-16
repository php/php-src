--TEST--
ldap_set_rebind_proc() - Basic ldap_set_rebind_proc test
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php
if (!function_exists('ldap_set_rebind_proc')) die("skip ldap_set_rebind_proc() not available");
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
/*** NOTE: THE CALLBACK IS NOT CALLED AS WE DON'T TEST THE REBINDING HAPPENS AS WE NEED MULTIPLE LDAP SERVERS ***/

require "connect.inc";

function rebind_proc ($ldap, $referral) {
    global $user;
    global $passwd;
    global $protocol_version;

    // required by most modern LDAP servers, use LDAPv3
    ldap_set_option($ldap, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

    if (!ldap_bind($ldap, $user, $passwd)) {
        // Failure
        print "Cannot bind";
        return 1;
    }
    // Success
    return 0;
}

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, 3);
ldap_set_option($link, LDAP_OPT_REFERRALS, true);

var_dump(ldap_set_rebind_proc($link, "rebind_proc"));
var_dump(ldap_set_rebind_proc($link, null));
?>
--EXPECT--
bool(true)
bool(true)
