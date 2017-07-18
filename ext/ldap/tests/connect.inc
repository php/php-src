<?php

/*
Default values are "localhost", "cn=Manager,dc=my-domain,dc=com", and password "secret".
Change the LDAP_TEST_* environment values if you want to use another configuration.
*/

$host			= getenv("LDAP_TEST_HOST")	? getenv("LDAP_TEST_HOST")	: "localhost";
$port			= getenv("LDAP_TEST_PORT")	? getenv("LDAP_TEST_PORT")	: 389;
$base			= getenv("LDAP_TEST_BASE")	? getenv("LDAP_TEST_BASE")	: "dc=my-domain,dc=com";
$user			= getenv("LDAP_TEST_USER")	? getenv("LDAP_TEST_USER")	: "cn=Manager,$base";
$sasl_user		= getenv("LDAP_TEST_SASL_USER")	? getenv("LDAP_TEST_SASL_USER")	: "Manager";
$passwd			= getenv("LDAP_TEST_PASSWD")	? getenv("LDAP_TEST_PASSWD")	: "secret";
$protocol_version	= getenv("LDAP_TEST_OPT_PROTOCOL_VERSION")	? getenv("LDAP_TEST_OPT_PROTOCOL_VERSION")	: 3;
$skip_on_bind_failure	= getenv("LDAP_TEST_SKIP_BIND_FAILURE") ? getenv("LDAP_TEST_SKIP_BIND_FAILURE") : true;

function ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version) {
	$link = ldap_connect($host, $port);
	ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
	ldap_bind($link, $user, $passwd);
	return $link;
}

function test_bind($host, $port, $user, $passwd, $protocol_version) {
	$link = ldap_connect($host, $port);
	ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
	return ldap_bind($link, $user, $passwd);
}

function insert_dummy_data($link, $base) {
	// Create root if not there
	$testBase = ldap_read($link, $base, '(objectClass=*)', array('objectClass'));
	if (ldap_count_entries($link, $testBase) < 1) {
		ldap_add(
			$link, "$base", array(
				"objectClass"   => array(
					"top",
					"organization",
					"dcObject"
				),
				"o" => "php ldap tests"
			)
		);
	}
	ldap_add($link, "o=test,$base", array(
	        "objectClass"   => array(
            "top",
            "organization"),
	        "o"             => "test",
	));
	ldap_add($link, "cn=userA,$base", array(
	        "objectclass" => "person",
	        "cn" => "userA",
	        "sn" => "testSN1",
	        "userPassword" => "oops",
	        "telephoneNumber" => "xx-xx-xx-xx-xx",
	        "description" => "user A",
	));
	ldap_add($link, "cn=userB,$base", array(
	        "objectclass" => "person",
	        "cn" => "userB",
	        "sn" => "testSN2",
	        "userPassword" => "oopsIDitItAgain",
	        "description" => "user B",
	));
	ldap_add($link, "cn=userC,cn=userB,$base", array(
	        "objectclass" => "person",
	        "cn" => "userC",
	        "sn" => "testSN3",
	        "userPassword" => "0r1g1na1 passw0rd",
	));
}

function remove_dummy_data($link, $base) {
	ldap_delete($link, "cn=userC,cn=userB,$base");
	ldap_delete($link, "cn=userA,$base");
	ldap_delete($link, "cn=userB,$base");
	ldap_delete($link, "o=test,$base");
}
?>
