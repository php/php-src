--TEST--
FDF POST data parsing
--SKIPIF--
<?php if (php_sapi_name()=='cli' || !extension_loaded("fdf")) print "skip"; ?>
--HEADERS--
return <<<END
Content-Type=application/vnd.fdf
Content-Length=209
END;
--ENV--
return <<<END
Content-Type=application/vnd.fdf
Content-Length=209
END;
--POST--
%FDF-1.2
%âãÏÓ

1 0 obj
<< 
/FDF << /Fields [ << /V (Thanks George)/T (status)>> ] /ID [ <3c0e51bf6427b09f7faa482297af6957><5f9787a1646a3bfe44b7725c9c1284df>
] >> 
>> 
endobj
trailer
<<
/Root 1 0 R 

>>
%%EOF
--FILE--
<?php 
echo strlen($HTTP_FDF_DATA)."\n";
foreach ($_POST as $key => $value) {
	echo "$key => $value\n";
}
?>
--EXPECT--
209
status: Thanks George