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
            echo "Windows\n";
            /* On Windows it should be in the same dir as php.exe in most of the cases. */
            $php_path = dirname($php);

            if (is_dir($php_path) && file_exists("$php_path/php-cgi.exe") && is_executable("$php_path/php-cgi.exe")) {
                return "$php_path/php-cgi.exe";
            }
        } else {
            for ($i = 0; $i < 2; $i++) {
                $slash_pos = strrpos($php_path, "/");
                if ($slash_pos) {
                    $php_path = substr($php_path, 0, $slash_pos);
                } else {
                    echo "Failed to remove directory\n";
                    return FALSE;
                }
            }

            if ($php_path && is_dir($php_path) && file_exists($php_path."/cgi/php-cgi") && is_executable($php_path."/cgi/php-cgi")) {
                /* gotcha */
                return $php_path."/cgi/php-cgi";
            }
            echo "$php_path/cgi/php-cgi does not exist\n";
        }
        return false;
    }
    /* uhm? what's that then? */
    echo "not cli\n";
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
