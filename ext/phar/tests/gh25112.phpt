--TEST--
GH-25112: Phar::webPhar() CGI redirect drops PATH_INFO after rewriting to the index
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
REQUEST_URI=/sub/gh25112.phar/foo
SCRIPT_NAME=/sub/gh25112.phar
PATH_INFO=/foo
--FILE--
<?php
$fname = __DIR__ . '/gh25112.phar';
$phar = new Phar($fname);
$phar->addFromString('index.php', '<?php echo "ok\n"; ?>');
$phar->setStub('<?php
Phar::webPhar(null, null, null, [], function ($path) { return "/"; });
__HALT_COMPILER(); ?>');
unset($phar);
include $fname;
?>
--CLEAN--
<?php @unlink(__DIR__ . '/gh25112.phar'); ?>
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /sub/gh25112.phar/index.php
--EXPECT--
