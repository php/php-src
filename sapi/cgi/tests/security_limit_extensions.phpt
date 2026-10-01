--TEST--
Tests for `cgi.security_limit_extensions`
--SKIPIF--
<?php include "skipif.inc"; ?>
--INI--
display_errors=stdout
--FILE--
<?php
include "include.inc";

reset_env_vars();

function check_file(string $path, ?string $limit = null) {
    $php = get_cgi_path();
    $ini = ($limit === null) ? '' : "-d cgi.security_limit_extensions='$limit'";
    echo "$path, ini=$ini:\n";
    echo trim(shell_exec("\"$php\" -n $ini -l \"$path\""));
    echo "\n\n";
}

echo "Default: allowed are .php and .phar:\n";
check_file(__FILE__);
check_file(__DIR__ . '/missing.test.php');
check_file(__DIR__ . '/missing.phar');
check_file(__DIR__ . '/include.inc');

echo "\n\n";
echo "Configured extensions:\n";
check_file(__FILE__, '.inc .phar .php');
check_file(__FILE__, ".inc\t  \t.phar \t\t .php");

echo "\n\n";
echo "Multiple parts to extension:\n";
check_file(__FILE__, '.test.php');
check_file(__DIR__ . '/missing.test.php', '.test.php');

echo "\n\n";
echo "Allowed longer than path, then .php:\n";
check_file(__FILE__, '.' . str_repeat('x', strlen(__FILE__)) . 'yz .php');

?>
--EXPECTF--
Default: allowed are .php and .phar:
%ssecurity_limit_extensions.php, ini=:
No syntax errors detected in %ssecurity_limit_extensions.php

%smissing.test.php, ini=:
No input file specified.

%smissing.phar, ini=:
No input file specified.

%sinclude.inc, ini=:
Access denied.



Configured extensions:
%ssecurity_limit_extensions.php, ini=-d cgi.security_limit_extensions='.inc .phar .php':
No syntax errors detected in %ssecurity_limit_extensions.php

%ssecurity_limit_extensions.php, ini=-d cgi.security_limit_extensions='.inc	  	.phar 		 .php':
No syntax errors detected in %ssecurity_limit_extensions.php



Multiple parts to extension:
%ssecurity_limit_extensions.php, ini=-d cgi.security_limit_extensions='.test.php':
Access denied.

%smissing.test.php, ini=-d cgi.security_limit_extensions='.test.php':
No input file specified.



Allowed longer than path, then .php:
%ssecurity_limit_extensions.php, ini=-d cgi.security_limit_extensions='.x%rx+%ryz .php':
No syntax errors detected in %ssecurity_limit_extensions.php
