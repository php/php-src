--TEST--
Deprecated GET/POST sessions
--EXTENSIONS--
session
--SKIPIF--
<?php include 'skipif.inc'; ?>
--INI--
session.use_cookies=0
session.use_only_cookies=1
session.use_trans_sid=0
--FILE--
<?php

ob_start();

// Expecting deprecation here
ini_set("session.use_only_cookies", "0");
// Expecting no deprecation
ini_set("session.use_only_cookies", "1");

// Expecting deprecation here
ini_set("session.use_trans_sid", "1");
// Expecting no deprecation
ini_set("session.use_trans_sid", "0");

// Expecting deprecation here
ini_set("session.trans_sid_tags", "a=href");
// Expecting no deprecation (default value)
ini_set("session.trans_sid_tags", "a=href,area=href,frame=src,form=");

// Expecting deprecation here
ini_set("session.trans_sid_hosts", "php.net");
// Expecting no deprecation (default value)
ini_set("session.trans_sid_hosts", "");

// Expecting deprecation here
ini_set("session.referer_check", "php.net");
// Expecting no deprecation (default value)
ini_set("session.referer_check", "");

// Setting deprecated values directly in session_start()
// Expecting deprecation here
session_start([ 'use_cookies' => '0', 'use_only_cookies' => '0', 'use_trans_sid' => '1']);

echo SID;

?>
--EXPECTF--
Deprecated: ini_set(): Disabling session.use_only_cookies INI setting is deprecated in %s on line 6

Deprecated: ini_set(): Enabling session.use_trans_sid INI setting is deprecated in %s on line 11

Deprecated: ini_set(): Usage of session.trans_sid_tags INI setting is deprecated in %s on line 16

Deprecated: ini_set(): Usage of session.trans_sid_hosts INI setting is deprecated in %s on line 21

Deprecated: ini_set(): Usage of session.referer_check INI setting is deprecated in %s on line 26

Deprecated: session_start(): Disabling session.use_only_cookies INI setting is deprecated in %s on line 32

Deprecated: session_start(): Enabling session.use_trans_sid INI setting is deprecated in %s on line 32

Deprecated: Constant SID is deprecated in %s on line 34
PHPSESSID=%s
