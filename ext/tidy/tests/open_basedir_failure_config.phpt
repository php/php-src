--TEST--
Tidy with basedir restriction failure on configuration file
--EXTENSIONS--
tidy
--INI--
open_basedir={PWD}/open_basedir
--FILE--
<?php
echo "=== repairString ===\n";
$tidy = new tidy;
$tidy->repairString('my epic string', 'my_config_file.ini');

echo "=== tidy_parse_string ===\n";
tidy_parse_string('my epic string', 'my_config_file.ini');

echo "=== tidy_parse_file ===\n";
tidy_parse_file(__DIR__.'/open_basedir/test.html', 'my_config_file.ini');

echo "=== __construct ===\n";
$tidy = new tidy(__DIR__.'/open_basedir/test.html', 'my_config_file.ini');

echo "=== parseFile ===\n";
$tidy = new tidy;
$tidy->parseFile(__DIR__.'/open_basedir/test.html', 'my_config_file.ini');

echo "=== parseString ===\n";
$tidy = new tidy;
$tidy->parseString('my epic string', 'my_config_file.ini');
?>
--EXPECTF--
=== repairString ===

Warning: tidy::repairString(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
=== tidy_parse_string ===

Warning: tidy_parse_string(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
=== tidy_parse_file ===

Warning: tidy_parse_file(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
=== __construct ===

Warning: tidy::__construct(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
=== parseFile ===

Warning: tidy::parseFile(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
=== parseString ===

Warning: tidy::parseString(): open_basedir restriction in effect. File(my_config_file.ini) is not within the allowed path(s): (%sopen_basedir) in %s on line %d
