--TEST--
FPM: GHSA-54hq-v5wp-fqgv - exceeding max_file_uploads
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
php_admin_value[max_file_uploads] = 5
php_admin_flag[display_errors] = On
php_admin_flag[display_startup_errors] = On
php_admin_flag[log_errors] = On
EOT;

$code = <<<EOT
<?php
var_dump(count(\$_FILES));
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
echo $tester
    ->request(stdin: [
        'parts' => [
            'count' => 10,
            'param' => 'filename'
        ]
    ])
    ->getBody();
$tester->terminate();
$tester->close();

?>
--EXPECT--
Warning: Maximum number of allowable file uploads has been exceeded in Unknown on line 0
int(5)
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
