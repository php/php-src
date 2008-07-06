--TEST--
CLI -a and readline 
--SKIPIF--
<?php 
include "skipif.inc"; 
if (!extension_loaded('readline') || readline_info('done') === NULL) {
	die ("skip need readline support");
}
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');

// disallow console escape sequences that may break the output
putenv('TERM=VT100');

$codes = array();

$codes[1] = <<<EOT
echo 'Hello world';
exit
EOT;

$codes[] = <<<EOT
echo 'multine
single
quote';
exit
EOT;

$codes[] = <<<EOT
echo <<<HEREDOC
Here
comes
the
doc
HEREDOC;
EOT;

$codes[] = <<<EOT
if (0) {
    echo "I'm not there";
}
echo "Done";
EOT;

$codes[] = <<<EOT
function a_function_with_some_name() {
    echo "I was called!";
}
a_function_w	);
EOT;

foreach ($codes as $key => $code) {
	echo "\n--------------\nSnippet no. $key:\n--------------\n";
	$code = escapeshellarg($code);
	echo `echo $code | "$php" -a`, "\n";
}

echo "\nDone\n";
?>
--EXPECTF--
--------------
Snippet no. 1:
--------------
Interactive shell

php > Hello world
php > 

--------------
Snippet no. 2:
--------------
Interactive shell

php > php ' php ' multine
single
quote
php > 

--------------
Snippet no. 3:
--------------
Interactive shell

php > <<< > <<< > <<< > <<< > <<< > Here
comes
the
doc
php > 

--------------
Snippet no. 4:
--------------
Interactive shell

php > php { php { php > Done
php > 

--------------
Snippet no. 5:
--------------
Interactive shell

php > php { php { php > I was called!
php > 

Done
