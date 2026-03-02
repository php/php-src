<?php

/*
By default tests will try to access SNMP agent @ '127.0.0.1:161' and will use 'public' community for read
requests and 'private' community for write requests.
Default timeout is 1000ms and there will be one request performed.
*/

$hostname4 =	getenv('SNMP_HOSTNAME')		?:	'127.0.0.1';
$hostname6 =	getenv('SNMP_HOSTNAME6')	?:	'::1';
$port =		getenv('SNMP_PORT')		?:		'161';
$hostname	= "$hostname4:$port";
$hostname6_port = "[$hostname6]:$port";
$community =	getenv('SNMP_COMMUNITY')	?:	'public';
$communityWrite = getenv('SNMP_COMMUNITY_WRITE')?:'private';

$timeout = 	getenv('SNMP_TIMEOUT') ?:	-1;
$retries = 	getenv('SNMP_RETRIES') ?:	1;

if (stristr(PHP_OS, "FreeBSD")) {
    $mibdir = 	getenv('SNMP_MIBDIR')	?:		"/usr/local/share/snmp/mibs";
} else {
    $mibdir = 	getenv('SNMP_MIBDIR')	?:		"/usr/share/snmp/mibs";
}


$user_noauth =	getenv('SNMP_USER_NOAUTH') ?:	'noAuthUser';
$user_auth_prefix = getenv('SNMP_USER_PREFIX') ?:	'admin';
$rwuser = 	getenv('SNMP_RWUSER') ?:		($user_auth_prefix . 'MD5AES');
$auth_pass =	getenv('SNMP_AUTH_PASS') ?:	'test1234';
$priv_pass =	getenv('SNMP_PRIV_PASS') ?:	'test1234';
