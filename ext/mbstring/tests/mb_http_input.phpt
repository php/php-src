--TEST--
mb_http_input()
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
(php_sapi_name()=='cgi') or die("skip sapi is not a cgi version");
die("skip disabled temporarily");
?>
--POST--
a=���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
--GET--
b=���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
--FILE--
<?php
// TODO: This is not a real test.... Need to change so that it does real testing
//$debug = true;
ini_set('include_path', __DIR__);
include_once('common.inc');

$ini = ini_get('mbstring.http_input');

// It must be url encoded....
// echo vars
echo $_POST['a']."\n";
echo $_GET['b']."\n";

// Get encoding
$enc = mb_http_input('P');

// check
if (empty($ini)) {
    // Must be pass
    if ($enc === 'pass') {
        echo "OK\n";
    }
    else {
        echo "NG\n";
    }
}
else {
    // Some encoding
    echo "This heppens when php.ini-dist is not used\n";
}

?>
--EXPECT--
���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
OK
