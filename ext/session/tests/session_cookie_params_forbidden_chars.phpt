--TEST--
session.cookie_path and session.cookie_domain reject cookie separators
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

var_dump(session_set_cookie_params(['path' => '/; Domain=evil.example']));
var_dump(session_set_cookie_params(['domain' => 'example.com; HttpOnly']));
var_dump(session_set_cookie_params(['path' => "/\r\nX-Injected: yes"]));
var_dump(ini_set('session.cookie_domain', "example.com\tevil"));

$params = session_get_cookie_params();
var_dump($params['path'], $params['domain']);

var_dump(session_set_cookie_params(['path' => '/app', 'domain' => 'example.com']));
$params = session_get_cookie_params();
var_dump($params['path'], $params['domain']);

?>
--EXPECTF--
Warning: session_set_cookie_params(): "session.cookie_path" must not contain any of the following characters ",; \t\r\n\013\014" in %s on line %d
bool(false)

Warning: session_set_cookie_params(): "session.cookie_domain" must not contain any of the following characters ",; \t\r\n\013\014" in %s on line %d
bool(false)

Warning: session_set_cookie_params(): "session.cookie_path" must not contain any of the following characters ",; \t\r\n\013\014" in %s on line %d
bool(false)

Warning: ini_set(): "session.cookie_domain" must not contain any of the following characters ",; \t\r\n\013\014" in %s on line %d
bool(false)
string(1) "/"
string(0) ""
bool(true)
string(4) "/app"
string(11) "example.com"
