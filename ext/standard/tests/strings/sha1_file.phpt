--TEST--
Test sha1_file() function with ASCII output and raw binary output. Based on ext/standard/tests/strings/md5_file.phpt
--FILE--
<?php


echo "*** Testing sha1_file() : basic functionality ***\n";

/* Creating an empty file */
if (($handle = fopen( "EmptyFileSHA1.txt", "w+")) == FALSE)
return false;

/* Creating a data file */
if (($handle2 = fopen( "DataFileSHA1.txt", "w+")) == FALSE)
return false;

/* Writing into file */
$filename = "DataFileSHA1.txt";
$content = "Add this to the file\n";
if (is_writable($filename)) {
  if (fwrite($handle2, $content) === FALSE) {
    echo "Cannot write to file ($filename)";
    exit;
  }
}

// close the files
fclose($handle);
fclose($handle2);

/* Testing error conditions */
echo "\n*** Testing for error conditions ***\n";

echo "\n-- No filename --\n";
try {
    var_dump( sha1_file("") );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- invalid filename --\n";
var_dump( sha1_file("rewncwYcn89q") );

echo "\n-- Scalar value as filename --\n";
var_dump( sha1_file(12) );

echo "\n-- NULL as filename --\n";
try {
    var_dump( sha1_file(NULL) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Hexadecimal Output for Empty file as Argument --\n";
var_dump( sha1_file("EmptyFileSHA1.txt") );

echo "\n-- Raw Binary Output for Empty file as Argument --\n";
var_dump( bin2hex(sha1_file("EmptyFileSHA1.txt", true)));

echo "\n-- Hexadecimal Output for a valid file with some contents --\n";
var_dump( sha1_file("DataFileSHA1.txt") );

echo "\n-- Raw Binary Output for a valid file with some contents --\n";
var_dump ( bin2hex(sha1_file("DataFileSHA1.txt", true)));

// remove temp files
unlink("DataFileSHA1.txt");
unlink("EmptyFileSHA1.txt");

?>
--EXPECTF--
*** Testing sha1_file() : basic functionality ***

*** Testing for error conditions ***

-- No filename --
Path cannot be empty

-- invalid filename --

Warning: sha1_file(rewncwYcn89q): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Scalar value as filename --

Warning: sha1_file(12): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- NULL as filename --
Path cannot be empty

-- Hexadecimal Output for Empty file as Argument --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"

-- Raw Binary Output for Empty file as Argument --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"

-- Hexadecimal Output for a valid file with some contents --
string(40) "d16a568ab98233deff7ec8b1668eb4b3d9e53fee"

-- Raw Binary Output for a valid file with some contents --
string(40) "d16a568ab98233deff7ec8b1668eb4b3d9e53fee"
