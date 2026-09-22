--TEST--
session_start() when the SAPI rejects the session cookie header
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

set_error_handler(function (int $errno, string $errstr): bool {
    echo "handler: ", $errstr, PHP_EOL;
    return true;
});

session_set_cookie_params(['path' => "/\r\nX-Injected: yes"]);

var_dump(session_start());
var_dump(session_status() === PHP_SESSION_NONE);

?>
--EXPECT--
handler: Header may not contain more than a single header, new line detected
bool(false)
bool(true)
