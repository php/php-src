--TEST--
ldap_set_rebind_proc() - Testing ldap_set_rebind_proc() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php
    if (!function_exists("ldap_set_rebind_proc")) {
        die("skip ldap_set_rebind_proc() does not exist");
    }
    require "connect.inc";
    $link = @fsockopen($uri);
    if (!$link) {
        die("skip no server listening");
    }
?>
--FILE--
<?php
require "connect.inc";

function rebind_proc ($ds, $ldap_url) {
  global $user;
  global $passwd;
  global $protocol_version;

  // required by most modern LDAP servers, use LDAPv3
  ldap_set_option($a, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

  if (!ldap_bind($a, $user, $passwd)) {
        print "Cannot bind";
  }
}

$link = ldap_connect($uri);
try {
    $result = ldap_set_rebind_proc($link, "rebind_proc_inexistent");
} catch(\TypeError $error) {
    echo $error->getMessage(), "\n";
}
?>
--EXPECT--
ldap_set_rebind_proc(): Argument #2 ($callback) must be a valid callback or null, function "rebind_proc_inexistent" not found or invalid function name
