--TEST--
Hash: Snefru256 algorithm
--FILE--
<?php
echo "'snefru' is alias for 'snefru256': ", var_export(hash("snefru", "") === hash("snefru256", ""), true), "\n";
// Source: Archived original SNEFRU.ZIP at https://www.schneier.com/books/applied-cryptography-source/
echo "*** Snefru 2.5a reference ***\n";
$vector = "";
do {
	echo hash("snefru256", $vector."\n"), "\n";
	$vector .= strlen($vector)+1;
} while (strlen($vector) < 10);

?>
--EXPECT--
'snefru' is alias for 'snefru256': true
*** Snefru 2.5a reference ***
2e02687f0d45d5b9b50cb68c3f33e6843d618a1aca2d06893d3eb4e3026b5732
bfea4a05a2a2ef15c736d114598a20b9d9bd4d66b661e6b05ecf6a7737bdc58c
ac677d69761ade3f189c7aef106d5fe7392d324e19cc76d5db4a2c05f2cc2cc5
061c76aa1db4a22c0e42945e26c48499b5400162e08c640be05d3c007c44793d
1e87fe1d9c927e9e24be85e3cc73359873541640a6261793ce5a974953113f5e
1b59927d85a9349a87796620fe2ff401a06a7ba48794498ebab978efc3a68912
28e9d9bc35032b68faeda88101ecb2524317e9da111b0e3e7094107212d9cf72
f7fff4ee74fd1b8d6b3267f84e47e007f029d13b8af7e37e34d13b469b8f248f
ee7d64b0102b2205e98926613b200185559d08be6ad787da717c968744e11af3
4ca72639e40e9ab9c0c3f523c4449b3911632d374c124d7702192ec2e4e0b7a3
