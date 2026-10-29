--TEST--
Request boundary resets the URL rewriter and keeps state unrelated to the request
--EXTENSIONS--
zend_test
--INI--
variables_order=EGPCS
url_rewriter.tags="a=href"
--FILE--
<?php
putenv('ZEND_TEST_REQUEST_BOUNDARY=1');
var_dump(strtok('previous request', ' '));
@trigger_error('previous request', E_USER_WARNING);
output_add_rewrite_var('previous', 'request');
echo '<a href="page.php">link</a>', "\n";
$_GET['previous'] = 'request';
$_ENV['kept'] = 'by the script';

var_dump(zend_test_request_boundary());

var_dump($_GET);
var_dump($_ENV['kept']);
var_dump(error_get_last()['message']);
var_dump(getenv('ZEND_TEST_REQUEST_BOUNDARY'));
var_dump(strtok(' '));
var_dump(ob_list_handlers());
output_add_rewrite_var('current', 'request');
?>
<a href="page.php">link</a>
--EXPECT--
string(8) "previous"
<a href="page.php?previous=request">link</a>
bool(true)
array(0) {
}
string(13) "by the script"
string(16) "previous request"
string(1) "1"
string(7) "request"
array(0) {
}
<a href="page.php?current=request">link</a>
