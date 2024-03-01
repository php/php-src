--TEST--
PUT multipart
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
$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
move_uploaded_file($files[0]['tmp_name'], $file_path);
$file_content = file_get_contents($file_path);
unlink($file_path);
echo json_encode([
    'post' => $post,
    'files' => $files,
    'file_content' => $file_content,
    'post_global' => $_POST,
    'files_global' => $_FILES,
], JSON_PRETTY_PRINT);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
echo $tester
    ->request(method: 'PUT', stdin: [
        'parts' => [
            [
                "disposition" => "form-data",
                "param" => "name",
                "name" => "get_parameter",
                "value" => "foo",
            ],
            [
                "disposition" => "form-data",
                "param" => "filename",
                "name" => "uploaded_file",
                "value" => "bar",
            ],
        ],
    ])
    ->getBody();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--EXPECTF--
{
    "post": {
        "get_parameter": "foo"
    },
    "files": [
        {
            "name": "uploaded_file",
            "full_path": "uploaded_file",
            "type": "",
            "tmp_name": "%s",
            "error": 0,
            "size": 3
        }
    ],
    "file_content": "bar",
    "post_global": [
        "post_global"
    ],
    "files_global": [
        "files_global"
    ]
}
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
@unlink($file_path);
?>
