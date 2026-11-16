--TEST--
request_parse_body() post_max_size option
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<'EOT'
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_admin_value[post_max_size] = 10K
php_admin_value[html_errors] = 0
EOT;

$code = <<<'EOT'
<?php
[$post, $files] = request_parse_body(['post_max_size' => '100K']);
var_dump(strlen($post['field']));
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
echo $tester
    ->request(method: 'PUT', stdin: [
        'parts' => [[
            'name' => 'field',
            'value' => str_repeat('x', 50 * 1024),
        ]],
    ])
    ->getBody();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--EXPECT--
int(51200)
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
