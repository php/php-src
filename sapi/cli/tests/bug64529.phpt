--TEST--
Bug #64529 (Ran out of opcode space)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN") {
	die("skip non windows test");
}
exec('which expect', $output, $ret);
if ($ret) {
	die("skip no expect installed");
}
?>
--FILE--
<?php
$expect_executable = trim(`which expect`);
$php_executable = getenv('TEST_PHP_EXECUTABLE');
$script = __DIR__ . "/expect.sh";

if (extension_loaded("readline")) {
	$expect_script = <<<SCRIPT

set php_executable [lindex \$argv 0]

spawn \$php_executable -n -a

expect "php >"

send "echo 'hello world';\n"
send "\04"

expect eof

exit

SCRIPT;

} else {
	$expect_script = <<<SCRIPT

set php_executable [lindex \$argv 0]

spawn \$php_executable -n -a

expect "Interactive mode enabled"

send "<?php echo 'hello world';\n"
send "\04"

expect eof

exit

SCRIPT;
}

file_put_contents($script, $expect_script);

system($expect_executable . " " . $script . " " . $php_executable);

@unlink($script);
?>
--EXPECTF--
spawn %sphp -n -a
Interactive %s

%secho 'hello world';
%sello worl%s
