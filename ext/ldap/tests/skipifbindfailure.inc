<?php
require_once 'connect.inc';

if ($skip_on_bind_failure) {

	$link = ldap_connect($host, $port);
	ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
	if (!@ldap_bind($link, $user, $passwd))
		die(sprintf("skip Can't bind to LDAP Server - [%d] %s", ldap_errno($link), ldap_error($link)));

	ldap_unbind($link);
}
?>
