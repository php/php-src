--TEST--
Hash: haval algorithm (multi-vector, multi-pass, multi-width)
--FILE--
<?php
echo "Empty String\n";
for($pass=3; $pass<=5; $pass++)
	for($bits=128; $bits <= 256; $bits += 32) {
		$algo = sprintf('haval%d,%d',$bits,$pass);
		echo $algo . ': ' . hash($algo,'') . "\n";
	}

echo "\"abc\"\n";
for($pass=3; $pass<=5; $pass++)
	for($bits=128; $bits <= 256; $bits += 32) {
		$algo = sprintf('haval%d,%d',$bits,$pass);
		echo $algo . ': ' . hash($algo,'abc') . "\n";
	}

echo "\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789\"\n";
for($pass=3; $pass<=5; $pass++)
	for($bits=128; $bits <= 256; $bits += 32) {
		$algo = sprintf('haval%d,%d',$bits,$pass);
		echo $algo . ': ' . hash($algo,'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789') . "\n";
	}
--EXPECT--
Empty String
haval128,3: c68f39913f901f3ddf44c707357a7d70
haval160,3: d353c3ae22a25401d257643836d7231a9a95f953
haval192,3: e9c48d7903eaf2a91c5b350151efcb175c0fc82de2289a4e
haval224,3: c5aae9d47bffcaaf84a8c6e7ccacd60a0dd1932be7b1a192b9214b6d
haval256,3: 4f6938531f0bc8991f62da7bbd6f7de3fad44562b8c6f4ebf146d5b4e46f7c17
haval128,4: ee6bbf4d6a46a679b3a856c88538bb98
haval160,4: 1d33aae1be4146dbaaca0b6e70d7a11f10801525
haval192,4: 4a8372945afa55c7dead800311272523ca19d42ea47b72da
haval224,4: 3e56243275b3b81561750550e36fcd676ad2f5dd9e15f2e89e6ed78e
haval256,4: c92b2e23091e80e375dadce26982482d197b1a2521be82da819f8ca2c579b99b
haval128,5: 184b8482a0c050dca54b59c7f05bf5dd
haval160,5: 255158cfc1eed1a7be7c55ddd64d9790415b933b
haval192,5: 4839d0626f95935e17ee2fc4509387bbe2cc46cb382ffe85
haval224,5: 4a0513c032754f5582a758d35917ac9adf3854219b39e3ac77d1837e
haval256,5: be417bb4dd5cfb76c7126f4f8eeb1553a449039307b1a3cd451dbfdc0fbbe330
"abc"
haval128,3: 9e40ed883fb63e985d299b40cda2b8f2
haval160,3: b21e876c4d391e2a897661149d83576b5530a089
haval192,3: a7b14c9ef3092319b0e75e3b20b957d180bf20745629e8de
haval224,3: 5bc955220ba2346a948d2848eca37bdd5eca6ecca7b594bd32923fab
haval256,3: 8699f1e3384d05b2a84b032693e2b6f46df85a13a50d93808d6874bb8fb9e86c
haval128,4: 6f2132867c9648419adcd5013e532fa2
haval160,4: 77aca22f5b12cc09010afc9c0797308638b1cb9b
haval192,4: 7e29881ed05c915903dd5e24a8e81cde5d910142ae66207c
haval224,4: 124c43d2ba4884599d013e8c872bfea4c88b0b6bf6303974cbe04e68
haval256,4: 8f409f1bb6b30c5016fdce55f652642261575bedca0b9533f32f5455459142b5
haval128,5: d054232fe874d9c6c6dc8e6a853519ea
haval160,5: ae646b04845e3351f00c5161d138940e1fa0c11c
haval192,5: d12091104555b00119a8d07808a3380bf9e60018915b9025
haval224,5: 8081027a500147c512e5f1055986674d746d92af4841abeb89da64ad
haval256,5: 976cd6254c337969e5913b158392a2921af16fca51f5601d486e0a9de01156e7
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ0123456789"
haval128,3: ddf4304cc5ffa3db8aab60d4f8fc2a00
haval160,3: e709559359b15917623050e41d27a306c6c3a9db
haval192,3: 51e25280ad356c06f4b913b3cdb3abaaac5879dda0a4fea4
haval224,3: 28aa2c164e10bb3076574cc8aa8584fd6d04f6d82c37ea5c21e451b3
haval256,3: 5537364e3d75174b846d21adf9b113f9d8f97e4750df64d428c01e782f9ade4d
haval128,4: c7d981e8270e39888ba96cafe8745636
haval160,4: 3444e38cc2a132b818b554ced8f7d9592df28f57
haval192,4: 0ca58f140ed92828a27913ce5636611abcada220fccf3af7
haval224,4: a9d0571d0857773e71363e4e9dfcca4696dba3e5019e7225e65e0cb1
haval256,4: 1858d106bdc2fc787445364a163cfc6027597a45a58a2490d14203c8b9bdd268
haval128,5: d41e927ea041d2f0c255352b1a9f6195
haval160,5: f3245e222e6581d0c3077bd7af322af4b4fedab7
haval192,5: fc45dc17a7b19adfed2a6485921f7af7951d70703b9357c1
haval224,5: 29687958a6f0d54d495105df00dbda0153ee0f5708408db68a5bbea5
haval256,5: f93421623f852ac877584d1e4bba5d9345a95f81bfd277fe36dfeed1815f83d5
