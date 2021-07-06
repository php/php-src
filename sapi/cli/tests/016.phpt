--TEST--
CLI -a and readline
--EXTENSIONS--
readline
--SKIPIF--
<?php
include "skipif.inc";
if (readline_info('done') === NULL) {
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
--EXPECT--
--------------
Snippet no. 1:
--------------
Interactive shell

php > echo 'Hello world';
Hello world
php > exit


--------------
Snippet no. 2:
--------------
Interactive shell

php > echo 'multine
php ' single
php ' quote';
multine
single
quote
php > exit


--------------
Snippet no. 3:
--------------
Interactive shell

php > echo <<<HEREDOC
<<< > Here
<<< > comes
<<< > the
<<< > doc
<<< > HEREDOC;
Here
comes
the
doc
php > 

--------------
Snippet no. 4:
--------------
Interactive shell

php > if (0) {
php {     echo "I'm not there";
php { }
php > echo "Done";
Done
php > 

--------------
Snippet no. 5:
--------------
Interactive shell

php > function a_function_with_some_name() {
php {     echo "I was called!";
php { }
php > a_function_with_some_name();
I was called!
php > 

Done
