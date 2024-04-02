--TEST--
PUT x-www-form-urlencoded
--EXTENSIONS--
zend_test
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
EOT;

$code = <<<'EOT'
<?php
$_POST = ['post_global'];
$_FILES = ['files_global'];
[$post, $files] = request_parse_body();
echo json_encode([
    'post' => $post,
    'files' => $files,
    'post_global' => $_POST,
    'files_global' => $_FILES,
], JSON_PRETTY_PRINT);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
echo $tester
    ->request(
        method: 'PUT',
        headers: ['CONTENT_TYPE' => 'application/x-www-form-urlencoded'],
        stdin: 'foo=foo&bar[]=1&bar[]=2'
    )
    ->getBody();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
@unlink($file_path);
?>
--EXPECT--
{
    "post": {
        "foo": "foo",
        "bar": [
            "1",
            "2"
        ]
    },
    "files": [],
    "post_global": [
        "post_global"
    ],
    "files_global": [
        "files_global"
    ]
}
