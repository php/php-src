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
'new-blocklevel-tags' => 'wps:block,wps:var,wps:value',
'newline' => 'LF'
);

$tidy = tidy_parse_string($contents, $config, 'utf8');
$tidy->cleanRepair();

var_dump($tidy->value);

?>
--EXPECTF--
string(11%d) "<html>
<head>
<title></title>
</head>
<body>
<wps:block>%w<wps:var>
<wps:value></wps:var>%w</wps:block>
</body>
</html>"
