--TEST--
Logical filter: boolean
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$booleans = array(
'1' => true,
'On' => true,
'Off' => true,
'True' => true,
'TrUe' => true,
'oN' => true,

'0' => false,
'Off' => false,
'False' => false,
'faLsE' => false,
'oFf' => false,

'' => false
);

foreach($booleans as $val=>$exp) {
    $res =filter_var($val, FILTER_VALIDATE_BOOLEAN);
	    if ($res !== $exp) {
        echo "$val failed,'$exp' expect, '$res' received.\n";
    }
}
echo "Ok.";
?>
--EXPECT--
Ok.
