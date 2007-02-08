--TEST--
libtidy handling of 'new-blocklevel-tags'
--SKIPIF--
<?php
if (!extension_loaded('tidy')) die('skip');
if (strtotime(tidy_get_release()) < strtotime('20 january 2007')) die ('skip old libtidy');
?>
--FILE--
<?php

// more info at http://sf.net/tracker/?func=detail&atid=390963&aid=1598422&group_id=27659

$contents = '
<wps:block>
<wps:var>
<wps:value/>
</wps:var>
</wps:block>';

$config = array(
'new-blocklevel-tags' => 'wps:block,wps:var,wps:value'
);

$tidy = tidy_parse_string($contents, $config, 'utf8');
$tidy->cleanRepair();

var_dump($tidy->value);

?>
--EXPECT--
string(117) "<html>
<head>
<title></title>
</head>
<body>
<wps:block>
<wps:var>
<wps:value></wps:var>
</wps:block>
</body>
</html>"
--UEXPECT--
unicode(117) "<html>
<head>
<title></title>
</head>
<body>
<wps:block>
<wps:var>
<wps:value></wps:var>
</wps:block>
</body>
</html>"
