<?php

require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'config.inc');

/* Overrule global settings, if need be */
$config = [];
[
    'PDO_MYSQL_TEST_DSN' => $config['PDOTEST_DSN'],
    'PDO_MYSQL_TEST_USER' => $config['PDOTEST_USER'],
    'PDO_MYSQL_TEST_PASS' => $config['PDO_MYSQL_TEST_PASS'],
] = $env;

if (array_key_exists('PDO_MYSQL_TEST_ATTR', $env)) {
    $config['PDOTEST_ATTR'] = $env['PDO_MYSQL_TEST_ATTR'];
}

foreach ($config as $k => $v) {
    putenv("$k=$v");
}

/* MySQL specific settings */
foreach ($env as $k => $v) {
    define($k, $v);
}

if (!function_exists('sys_get_temp_dir')) {
    function sys_get_temp_dir() {

        if (!empty($_ENV['TMP']))
            return realpath( $_ENV['TMP'] );
        if (!empty($_ENV['TMPDIR']))
            return realpath( $_ENV['TMPDIR'] );
        if (!empty($_ENV['TEMP']))
            return realpath( $_ENV['TEMP'] );

        $temp_file = tempnam(md5(uniqid(rand(), TRUE)), '');
        if ($temp_file) {
            $temp_dir = realpath(dirname($temp_file));
            unlink($temp_file);
            return $temp_dir;
        }
        return FALSE;
    }
}
?>
