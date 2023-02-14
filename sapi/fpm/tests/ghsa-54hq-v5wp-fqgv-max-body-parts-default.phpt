--TEST--
FPM: GHSA-54hq-v5wp-fqgv - max_multipart_body_parts ini default
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_admin_value[html_errors] = false
php_admin_value[max_input_vars] = 20
php_admin_value[max_file_uploads] = 5
php_admin_flag[display_errors] = On
php_admin_flag[display_startup_errors] = On
php_admin_flag[log_errors] = On
EOT;

$code = <<<EOT
<?php
var_dump(count(\$_POST));
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
echo $tester
    ->request(stdin: [
        'parts' => [
            'count' => 30,
        ]
    ])
    ->getBody();
$tester->terminate();
$tester->close();

?>
--EXPECT--
Warning: Unknown: Input variables exceeded 20. To increase the limit change max_input_vars in php.ini. in Unknown on line 0

Warning: Unknown: Multipart body parts limit exceeded 25. To increase the limit change max_multipart_body_parts in php.ini. in Unknown on line 0
int(20)
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
