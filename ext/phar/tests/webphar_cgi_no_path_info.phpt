--TEST--
Phar::webPhar() double free in CGI when SCRIPT_NAME is set but PATH_INFO is absent
--CGI--
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
variables_order=EGPC
register_argc_argv=0
cgi.fix_pathinfo=0
--ENV--
REQUEST_METHOD=GET
SCRIPT_NAME=/webphar_cgi_no_path_info.phar
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$phar = new Phar($fname);
$phar->addFromString('index.php', '<?php echo "ok\n"; ?>');
$phar->setStub('<?php
Phar::webPhar();
__HALT_COMPILER(); ?>');
unset($phar);
include $fname;
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /webphar_cgi_no_path_info.phar/index.php
--EXPECT--
