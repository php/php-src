--TEST--
Invalid session IDs are not sent in the session cookie
--EXTENSIONS--
session
--INI--
session.save_handler=files
session.save_path=
session.use_cookies=1
session.use_strict_mode=0
--CGI--
--FILE--
<?php
session_id("nulbug\0stillvalid");
var_dump(session_start());
foreach (headers_list() as $header) {
    if (str_starts_with($header, "Set-Cookie:")) {
        echo $header, "\n";
    }
}
?>
--EXPECTF--
Warning: session_start(): Session ID is too long or contains illegal characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d

Warning: session_start(): Failed to read session data: files (path: ) in %s on line %d
bool(false)
