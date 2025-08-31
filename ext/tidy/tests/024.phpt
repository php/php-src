--TEST--
libtidy handling of 'new-blocklevel-tags'
--EXTENSIONS--
tidy
--FILE--
<?php

// more info at http://sf.net/tracker/?func=detail&atid=390963&aid=1598422&group_id=27659

$contents = '
<wps:block>
<wps:var>
<wps:value></wps:value>
</wps:var>
</wps:block>';

$config = array(
'doctype' => 'omit',
'new-blocklevel-tags' => 'wps:block,wps:var,wps:value',
'newline' => 'LF'
);

$tidy = tidy_parse_string($contents, $config, 'utf8');
$tidy->cleanRepair();
echo $tidy;

?>
--EXPECTF--
<html>
<head>
<title></title>
</head>
<body>
<wps:block>%w<wps:var>%w<wps:value></wps:value>%w</wps:var>%w</wps:block>
</body>
</html>
