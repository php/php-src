--TEST--
mysqli_quote_string()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once 'connect.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

echo mysqli_quote_string($link, '\\') . "\n";
echo mysqli_quote_string($link, '"') . "\n";
echo mysqli_quote_string($link, "'") . "\n";

$escaped = mysqli_quote_string($link, "\' \ \"");
echo $escaped . "\n";
$result = $link->query("SELECT $escaped AS test");
$value = $result->fetch_column();
echo $value . "\n";

$escaped = mysqli_quote_string($link, '" OR 1=1 -- foo');
echo $escaped . "\n";
$result = $link->query("SELECT $escaped AS test");
$value = $result->fetch_column();
echo $value . "\n";

$escaped = mysqli_quote_string($link, "\n");
if ($escaped !== "'\\n'") {
    printf("[001] Expected '\\n', got %s\n", $escaped);
}

$escaped =  mysqli_quote_string($link, "\r");
if ($escaped !== "'\\r'") {
    printf("[002] Expected '\\r', got %s\n", $escaped);
}

$escaped =  mysqli_quote_string($link, "foo" . chr(0) . "bar");
if ($escaped !== "'foo\\0bar'") {
    printf("[003] Expected 'foo\\0bar', got %s\n", $escaped);
}

// Test that the SQL injection is impossible with NO_BACKSLASH_ESCAPES mode
$link->query('SET @@sql_mode="NO_BACKSLASH_ESCAPES"');

echo $link->quote_string('\\') . "\n";
echo $link->quote_string('"') . "\n";
echo $link->quote_string("'") . "\n";

$escaped = $link->quote_string("\' \ \"");
echo $escaped . "\n";
$result = $link->query("SELECT $escaped AS test");
$value = $result->fetch_column();
echo $value . "\n";

$escaped = $link->quote_string('" OR 1=1 -- foo');
echo $escaped . "\n";
$result = $link->query("SELECT $escaped AS test");
$value = $result->fetch_column();
echo $value . "\n";

echo "done!";
?>
--EXPECT--
'\\'
'\"'
'\''
'\\\' \\ \"'
\' \ "
'\" OR 1=1 -- foo'
" OR 1=1 -- foo
'\'
'"'
''''
'\'' \ "'
\' \ "
'" OR 1=1 -- foo'
" OR 1=1 -- foo
done!
