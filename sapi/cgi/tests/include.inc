<?php

function get_cgi_path() /* {{{ */
{
    $php = getenv("TEST_PHP_EXECUTABLE");

    $cli = false;
    $cgi = false;

    if (file_exists($php) && is_executable($php)) {
        $version = `$php -n -v`;
        if (strstr($version, "(cli)")) {
            /* that's cli */
            $cli = true;
        } else if (strpos($version, "(cgi")) {
            /* that's cgi */
            return $php;
        }
    }

    if ($cli) {
        /* trying to guess ... */
        $php_path = $php;
        if (defined("PHP_WINDOWS_VERSION_MAJOR")) {
            /* On Windows it should be in the same dir as php.exe in most of the cases. */
            $cgi_path = dirname($php) . "/php-cgi.exe";
            if (is_executable($cgi_path)) {
                return $cgi_path;
            }
        } else {
            /* Try in the same path as php, for the case where php is installed. */
            $cgi_path = dirname($php) . "/php-cgi";
            if (is_executable($cgi_path)) {
                return $cgi_path;
            }

            /* Try sapi/cgi/php-cgi, for the case where php is not installed. */
            $cgi_path = dirname($php, 3) . "/sapi/cgi/php-cgi";
            if (is_executable($cgi_path)) {
                return $cgi_path;
            }
        }
        return false;
    }
    /* uhm? what's that then? */
    return false;
}
/* }}} */

function reset_env_vars() /* {{{ */
{
    putenv("REDIRECT_STATUS");
    putenv("QUERY_STRING");
    putenv("PATH_TRANSLATED");
    putenv("SCRIPT_FILENAME");
    putenv("SERVER_SOFTWARE");
    putenv("SERVER_NAME");
    putenv("GATEWAY_INTERFACE");
    putenv("REQUEST_METHOD");
}
/* }}} */

?>
