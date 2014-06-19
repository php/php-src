--TEST--
Bug #44461 (parse_ini_file crashes)
--FILE--
<?php
file_put_contents(__DIR__ . 'bug44461.ini', <<<EOF
[attachments]
zip = "application/zip" ; MIME-type for ZIP files
EOF
);
parse_ini_file(__DIR__ . 'bug44461.ini', true);
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . 'bug44461.ini');
?>
--EXPECT--
===DONE===