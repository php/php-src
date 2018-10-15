--TEST--
sha3 algorithm
--SKIPIF--
<?php if(!extension_loaded("hash")) print "skip"; ?>
--FILE--
<?php

// Test vectors taken from a combination of NIST FIPS-202,
// Wikipedia reference vectors,
// and output from reference implementation
$subjects = [
  '',
  'a',
  'The quick brown fox jumps over the lazy dog',
  'The quick brown fox jumps over the lazy dog.',
  str_repeat('a', 257),
  str_repeat("\xA3", 200),
];
foreach ($subjects as $subject) {
  echo '== ', urlencode($subject), " ==\n";
  foreach ([224, 256, 384, 512] as $bits) {
    echo $bits, ': ', hash("sha3-$bits", $subject), "\n";
  }
}
--EXPECT--
==  ==
224: 6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7
256: a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a
384: 0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004
512: a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26
== a ==
224: 9e86ff69557ca95f405f081269685b38e3a819b309ee942f482b6a8b
256: 80084bf2fba02475726feb2cab2d8215eab14bc6bdd8bfb2c8151257032ecd8b
384: 1815f774f320491b48569efec794d249eeb59aae46d22bf77dafe25c5edc28d7ea44f93ee1234aa88f61c91912a4ccd9
512: 697f2d856172cb8309d6b8b97dac4de344b549d4dee61edfb4962d8698b7fa803f4f93ff24393586e28b5b957ac3d1d369420ce53332712f997bd336d09ab02a
== The+quick+brown+fox+jumps+over+the+lazy+dog ==
224: d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795
256: 69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04
384: 7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41
512: 01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450
== The+quick+brown+fox+jumps+over+the+lazy+dog. ==
224: 2d0708903833afabdd232a20201176e8b58c5be8a6fe74265ac54db0
256: a80f839cd4f83f6c3dafc87feae470045e4eb0d366397d5c6ce34ba1739f734d
384: 1a34d81695b622df178bc74df7124fe12fac0f64ba5250b78b99c1273d4b080168e10652894ecad5f1f4d5b965437fb9
512: 18f4f4bd419603f95538837003d9d254c26c23765565162247483f65c50303597bc9ce4d289f21d1c2f1f458828e33dc442100331b35e7eb031b5d38ba6460f8
== aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ==
224: 10dd422d71c42ee102a0c4bd398b5b85470341a0794702c954b022ba
256: 6d115e8744deef792419e8bdb8567d74844e0fa5c2d5e474a19de87ac001449f
384: c9df41fa389101cde63447257835464d89fd3974e5813f3f58d30e0296e89486e2d4bfc2b4089cd3bb860a20263322b8
512: 5008048b64c14975181175f157be4a780c3d443d2177edf323d57884bc7e3979b9b53bca1325e880df3da0d97c435693441cb5527fbe950f5585678dfbb37785
== %A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3%A3 ==
224: 9376816aba503f72f96ce7eb65ac095deee3be4bf9bbc2a1cb7e11e0
256: 79f38adec5c20307a98ef76e8324afbfd46cfd81b22e3973c65fa1bd9de31787
384: 1881de2ca7e41ef95dc4732b8f5f002b189cc1e42b74168ed1732649ce1dbcdd76197a31fd55ee989f2d7050dd473e8f
512: e76dfad22084a8b1467fcf2ffa58361bec7628edf5f3fdc0e4805dc48caeeca81b7c13c30adf52a3659584739a2df46be589c51ca1a4a8416df6545a1ce8ba00
