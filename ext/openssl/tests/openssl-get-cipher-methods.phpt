--TEST--
array openssl_get_cipher_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
var_dump(openssl_get_cipher_methods(true));
var_dump(openssl_get_cipher_methods(false));
?>
--EXPECT--
array(203) {
  [0]=>
  string(11) "AES-128-CBC"
  [1]=>
  string(11) "AES-128-CFB"
  [2]=>
  string(12) "AES-128-CFB1"
  [3]=>
  string(12) "AES-128-CFB8"
  [4]=>
  string(11) "AES-128-CTR"
  [5]=>
  string(11) "AES-128-ECB"
  [6]=>
  string(11) "AES-128-OFB"
  [7]=>
  string(11) "AES-128-XTS"
  [8]=>
  string(11) "AES-192-CBC"
  [9]=>
  string(11) "AES-192-CFB"
  [10]=>
  string(12) "AES-192-CFB1"
  [11]=>
  string(12) "AES-192-CFB8"
  [12]=>
  string(11) "AES-192-CTR"
  [13]=>
  string(11) "AES-192-ECB"
  [14]=>
  string(11) "AES-192-OFB"
  [15]=>
  string(11) "AES-256-CBC"
  [16]=>
  string(11) "AES-256-CFB"
  [17]=>
  string(12) "AES-256-CFB1"
  [18]=>
  string(12) "AES-256-CFB8"
  [19]=>
  string(11) "AES-256-CTR"
  [20]=>
  string(11) "AES-256-ECB"
  [21]=>
  string(11) "AES-256-OFB"
  [22]=>
  string(11) "AES-256-XTS"
  [23]=>
  string(6) "AES128"
  [24]=>
  string(6) "AES192"
  [25]=>
  string(6) "AES256"
  [26]=>
  string(2) "BF"
  [27]=>
  string(6) "BF-CBC"
  [28]=>
  string(6) "BF-CFB"
  [29]=>
  string(6) "BF-ECB"
  [30]=>
  string(6) "BF-OFB"
  [31]=>
  string(16) "CAMELLIA-128-CBC"
  [32]=>
  string(16) "CAMELLIA-128-CFB"
  [33]=>
  string(17) "CAMELLIA-128-CFB1"
  [34]=>
  string(17) "CAMELLIA-128-CFB8"
  [35]=>
  string(16) "CAMELLIA-128-ECB"
  [36]=>
  string(16) "CAMELLIA-128-OFB"
  [37]=>
  string(16) "CAMELLIA-192-CBC"
  [38]=>
  string(16) "CAMELLIA-192-CFB"
  [39]=>
  string(17) "CAMELLIA-192-CFB1"
  [40]=>
  string(17) "CAMELLIA-192-CFB8"
  [41]=>
  string(16) "CAMELLIA-192-ECB"
  [42]=>
  string(16) "CAMELLIA-192-OFB"
  [43]=>
  string(16) "CAMELLIA-256-CBC"
  [44]=>
  string(16) "CAMELLIA-256-CFB"
  [45]=>
  string(17) "CAMELLIA-256-CFB1"
  [46]=>
  string(17) "CAMELLIA-256-CFB8"
  [47]=>
  string(16) "CAMELLIA-256-ECB"
  [48]=>
  string(16) "CAMELLIA-256-OFB"
  [49]=>
  string(11) "CAMELLIA128"
  [50]=>
  string(11) "CAMELLIA192"
  [51]=>
  string(11) "CAMELLIA256"
  [52]=>
  string(4) "CAST"
  [53]=>
  string(8) "CAST-cbc"
  [54]=>
  string(9) "CAST5-CBC"
  [55]=>
  string(9) "CAST5-CFB"
  [56]=>
  string(9) "CAST5-ECB"
  [57]=>
  string(9) "CAST5-OFB"
  [58]=>
  string(3) "DES"
  [59]=>
  string(7) "DES-CBC"
  [60]=>
  string(7) "DES-CFB"
  [61]=>
  string(8) "DES-CFB1"
  [62]=>
  string(8) "DES-CFB8"
  [63]=>
  string(7) "DES-ECB"
  [64]=>
  string(7) "DES-EDE"
  [65]=>
  string(11) "DES-EDE-CBC"
  [66]=>
  string(11) "DES-EDE-CFB"
  [67]=>
  string(11) "DES-EDE-OFB"
  [68]=>
  string(8) "DES-EDE3"
  [69]=>
  string(12) "DES-EDE3-CBC"
  [70]=>
  string(12) "DES-EDE3-CFB"
  [71]=>
  string(13) "DES-EDE3-CFB1"
  [72]=>
  string(13) "DES-EDE3-CFB8"
  [73]=>
  string(12) "DES-EDE3-OFB"
  [74]=>
  string(7) "DES-OFB"
  [75]=>
  string(4) "DES3"
  [76]=>
  string(4) "DESX"
  [77]=>
  string(8) "DESX-CBC"
  [78]=>
  string(4) "IDEA"
  [79]=>
  string(8) "IDEA-CBC"
  [80]=>
  string(8) "IDEA-CFB"
  [81]=>
  string(8) "IDEA-ECB"
  [82]=>
  string(8) "IDEA-OFB"
  [83]=>
  string(3) "RC2"
  [84]=>
  string(10) "RC2-40-CBC"
  [85]=>
  string(10) "RC2-64-CBC"
  [86]=>
  string(7) "RC2-CBC"
  [87]=>
  string(7) "RC2-CFB"
  [88]=>
  string(7) "RC2-ECB"
  [89]=>
  string(7) "RC2-OFB"
  [90]=>
  string(3) "RC4"
  [91]=>
  string(6) "RC4-40"
  [92]=>
  string(12) "RC4-HMAC-MD5"
  [93]=>
  string(4) "SEED"
  [94]=>
  string(8) "SEED-CBC"
  [95]=>
  string(8) "SEED-CFB"
  [96]=>
  string(8) "SEED-ECB"
  [97]=>
  string(8) "SEED-OFB"
  [98]=>
  string(11) "aes-128-cbc"
  [99]=>
  string(11) "aes-128-cfb"
  [100]=>
  string(12) "aes-128-cfb1"
  [101]=>
  string(12) "aes-128-cfb8"
  [102]=>
  string(11) "aes-128-ctr"
  [103]=>
  string(11) "aes-128-ecb"
  [104]=>
  string(11) "aes-128-gcm"
  [105]=>
  string(11) "aes-128-ofb"
  [106]=>
  string(11) "aes-128-xts"
  [107]=>
  string(11) "aes-192-cbc"
  [108]=>
  string(11) "aes-192-cfb"
  [109]=>
  string(12) "aes-192-cfb1"
  [110]=>
  string(12) "aes-192-cfb8"
  [111]=>
  string(11) "aes-192-ctr"
  [112]=>
  string(11) "aes-192-ecb"
  [113]=>
  string(11) "aes-192-gcm"
  [114]=>
  string(11) "aes-192-ofb"
  [115]=>
  string(11) "aes-256-cbc"
  [116]=>
  string(11) "aes-256-cfb"
  [117]=>
  string(12) "aes-256-cfb1"
  [118]=>
  string(12) "aes-256-cfb8"
  [119]=>
  string(11) "aes-256-ctr"
  [120]=>
  string(11) "aes-256-ecb"
  [121]=>
  string(11) "aes-256-gcm"
  [122]=>
  string(11) "aes-256-ofb"
  [123]=>
  string(11) "aes-256-xts"
  [124]=>
  string(6) "aes128"
  [125]=>
  string(6) "aes192"
  [126]=>
  string(6) "aes256"
  [127]=>
  string(2) "bf"
  [128]=>
  string(6) "bf-cbc"
  [129]=>
  string(6) "bf-cfb"
  [130]=>
  string(6) "bf-ecb"
  [131]=>
  string(6) "bf-ofb"
  [132]=>
  string(8) "blowfish"
  [133]=>
  string(16) "camellia-128-cbc"
  [134]=>
  string(16) "camellia-128-cfb"
  [135]=>
  string(17) "camellia-128-cfb1"
  [136]=>
  string(17) "camellia-128-cfb8"
  [137]=>
  string(16) "camellia-128-ecb"
  [138]=>
  string(16) "camellia-128-ofb"
  [139]=>
  string(16) "camellia-192-cbc"
  [140]=>
  string(16) "camellia-192-cfb"
  [141]=>
  string(17) "camellia-192-cfb1"
  [142]=>
  string(17) "camellia-192-cfb8"
  [143]=>
  string(16) "camellia-192-ecb"
  [144]=>
  string(16) "camellia-192-ofb"
  [145]=>
  string(16) "camellia-256-cbc"
  [146]=>
  string(16) "camellia-256-cfb"
  [147]=>
  string(17) "camellia-256-cfb1"
  [148]=>
  string(17) "camellia-256-cfb8"
  [149]=>
  string(16) "camellia-256-ecb"
  [150]=>
  string(16) "camellia-256-ofb"
  [151]=>
  string(11) "camellia128"
  [152]=>
  string(11) "camellia192"
  [153]=>
  string(11) "camellia256"
  [154]=>
  string(4) "cast"
  [155]=>
  string(8) "cast-cbc"
  [156]=>
  string(9) "cast5-cbc"
  [157]=>
  string(9) "cast5-cfb"
  [158]=>
  string(9) "cast5-ecb"
  [159]=>
  string(9) "cast5-ofb"
  [160]=>
  string(3) "des"
  [161]=>
  string(7) "des-cbc"
  [162]=>
  string(7) "des-cfb"
  [163]=>
  string(8) "des-cfb1"
  [164]=>
  string(8) "des-cfb8"
  [165]=>
  string(7) "des-ecb"
  [166]=>
  string(7) "des-ede"
  [167]=>
  string(11) "des-ede-cbc"
  [168]=>
  string(11) "des-ede-cfb"
  [169]=>
  string(11) "des-ede-ofb"
  [170]=>
  string(8) "des-ede3"
  [171]=>
  string(12) "des-ede3-cbc"
  [172]=>
  string(12) "des-ede3-cfb"
  [173]=>
  string(13) "des-ede3-cfb1"
  [174]=>
  string(13) "des-ede3-cfb8"
  [175]=>
  string(12) "des-ede3-ofb"
  [176]=>
  string(7) "des-ofb"
  [177]=>
  string(4) "des3"
  [178]=>
  string(4) "desx"
  [179]=>
  string(8) "desx-cbc"
  [180]=>
  string(13) "id-aes128-GCM"
  [181]=>
  string(13) "id-aes192-GCM"
  [182]=>
  string(13) "id-aes256-GCM"
  [183]=>
  string(4) "idea"
  [184]=>
  string(8) "idea-cbc"
  [185]=>
  string(8) "idea-cfb"
  [186]=>
  string(8) "idea-ecb"
  [187]=>
  string(8) "idea-ofb"
  [188]=>
  string(3) "rc2"
  [189]=>
  string(10) "rc2-40-cbc"
  [190]=>
  string(10) "rc2-64-cbc"
  [191]=>
  string(7) "rc2-cbc"
  [192]=>
  string(7) "rc2-cfb"
  [193]=>
  string(7) "rc2-ecb"
  [194]=>
  string(7) "rc2-ofb"
  [195]=>
  string(3) "rc4"
  [196]=>
  string(6) "rc4-40"
  [197]=>
  string(12) "rc4-hmac-md5"
  [198]=>
  string(4) "seed"
  [199]=>
  string(8) "seed-cbc"
  [200]=>
  string(8) "seed-cfb"
  [201]=>
  string(8) "seed-ecb"
  [202]=>
  string(8) "seed-ofb"
}
array(172) {
  [0]=>
  string(11) "AES-128-CBC"
  [1]=>
  string(11) "AES-128-CFB"
  [2]=>
  string(12) "AES-128-CFB1"
  [3]=>
  string(12) "AES-128-CFB8"
  [4]=>
  string(11) "AES-128-CTR"
  [5]=>
  string(11) "AES-128-ECB"
  [6]=>
  string(11) "AES-128-OFB"
  [7]=>
  string(11) "AES-128-XTS"
  [8]=>
  string(11) "AES-192-CBC"
  [9]=>
  string(11) "AES-192-CFB"
  [10]=>
  string(12) "AES-192-CFB1"
  [11]=>
  string(12) "AES-192-CFB8"
  [12]=>
  string(11) "AES-192-CTR"
  [13]=>
  string(11) "AES-192-ECB"
  [14]=>
  string(11) "AES-192-OFB"
  [15]=>
  string(11) "AES-256-CBC"
  [16]=>
  string(11) "AES-256-CFB"
  [17]=>
  string(12) "AES-256-CFB1"
  [18]=>
  string(12) "AES-256-CFB8"
  [19]=>
  string(11) "AES-256-CTR"
  [20]=>
  string(11) "AES-256-ECB"
  [21]=>
  string(11) "AES-256-OFB"
  [22]=>
  string(11) "AES-256-XTS"
  [23]=>
  string(6) "BF-CBC"
  [24]=>
  string(6) "BF-CFB"
  [25]=>
  string(6) "BF-ECB"
  [26]=>
  string(6) "BF-OFB"
  [27]=>
  string(16) "CAMELLIA-128-CBC"
  [28]=>
  string(16) "CAMELLIA-128-CFB"
  [29]=>
  string(17) "CAMELLIA-128-CFB1"
  [30]=>
  string(17) "CAMELLIA-128-CFB8"
  [31]=>
  string(16) "CAMELLIA-128-ECB"
  [32]=>
  string(16) "CAMELLIA-128-OFB"
  [33]=>
  string(16) "CAMELLIA-192-CBC"
  [34]=>
  string(16) "CAMELLIA-192-CFB"
  [35]=>
  string(17) "CAMELLIA-192-CFB1"
  [36]=>
  string(17) "CAMELLIA-192-CFB8"
  [37]=>
  string(16) "CAMELLIA-192-ECB"
  [38]=>
  string(16) "CAMELLIA-192-OFB"
  [39]=>
  string(16) "CAMELLIA-256-CBC"
  [40]=>
  string(16) "CAMELLIA-256-CFB"
  [41]=>
  string(17) "CAMELLIA-256-CFB1"
  [42]=>
  string(17) "CAMELLIA-256-CFB8"
  [43]=>
  string(16) "CAMELLIA-256-ECB"
  [44]=>
  string(16) "CAMELLIA-256-OFB"
  [45]=>
  string(9) "CAST5-CBC"
  [46]=>
  string(9) "CAST5-CFB"
  [47]=>
  string(9) "CAST5-ECB"
  [48]=>
  string(9) "CAST5-OFB"
  [49]=>
  string(7) "DES-CBC"
  [50]=>
  string(7) "DES-CFB"
  [51]=>
  string(8) "DES-CFB1"
  [52]=>
  string(8) "DES-CFB8"
  [53]=>
  string(7) "DES-ECB"
  [54]=>
  string(7) "DES-EDE"
  [55]=>
  string(11) "DES-EDE-CBC"
  [56]=>
  string(11) "DES-EDE-CFB"
  [57]=>
  string(11) "DES-EDE-OFB"
  [58]=>
  string(8) "DES-EDE3"
  [59]=>
  string(12) "DES-EDE3-CBC"
  [60]=>
  string(12) "DES-EDE3-CFB"
  [61]=>
  string(13) "DES-EDE3-CFB1"
  [62]=>
  string(13) "DES-EDE3-CFB8"
  [63]=>
  string(12) "DES-EDE3-OFB"
  [64]=>
  string(7) "DES-OFB"
  [65]=>
  string(8) "DESX-CBC"
  [66]=>
  string(8) "IDEA-CBC"
  [67]=>
  string(8) "IDEA-CFB"
  [68]=>
  string(8) "IDEA-ECB"
  [69]=>
  string(8) "IDEA-OFB"
  [70]=>
  string(10) "RC2-40-CBC"
  [71]=>
  string(10) "RC2-64-CBC"
  [72]=>
  string(7) "RC2-CBC"
  [73]=>
  string(7) "RC2-CFB"
  [74]=>
  string(7) "RC2-ECB"
  [75]=>
  string(7) "RC2-OFB"
  [76]=>
  string(3) "RC4"
  [77]=>
  string(6) "RC4-40"
  [78]=>
  string(12) "RC4-HMAC-MD5"
  [79]=>
  string(8) "SEED-CBC"
  [80]=>
  string(8) "SEED-CFB"
  [81]=>
  string(8) "SEED-ECB"
  [82]=>
  string(8) "SEED-OFB"
  [83]=>
  string(11) "aes-128-cbc"
  [84]=>
  string(11) "aes-128-cfb"
  [85]=>
  string(12) "aes-128-cfb1"
  [86]=>
  string(12) "aes-128-cfb8"
  [87]=>
  string(11) "aes-128-ctr"
  [88]=>
  string(11) "aes-128-ecb"
  [89]=>
  string(11) "aes-128-gcm"
  [90]=>
  string(11) "aes-128-ofb"
  [91]=>
  string(11) "aes-128-xts"
  [92]=>
  string(11) "aes-192-cbc"
  [93]=>
  string(11) "aes-192-cfb"
  [94]=>
  string(12) "aes-192-cfb1"
  [95]=>
  string(12) "aes-192-cfb8"
  [96]=>
  string(11) "aes-192-ctr"
  [97]=>
  string(11) "aes-192-ecb"
  [98]=>
  string(11) "aes-192-gcm"
  [99]=>
  string(11) "aes-192-ofb"
  [100]=>
  string(11) "aes-256-cbc"
  [101]=>
  string(11) "aes-256-cfb"
  [102]=>
  string(12) "aes-256-cfb1"
  [103]=>
  string(12) "aes-256-cfb8"
  [104]=>
  string(11) "aes-256-ctr"
  [105]=>
  string(11) "aes-256-ecb"
  [106]=>
  string(11) "aes-256-gcm"
  [107]=>
  string(11) "aes-256-ofb"
  [108]=>
  string(11) "aes-256-xts"
  [109]=>
  string(6) "bf-cbc"
  [110]=>
  string(6) "bf-cfb"
  [111]=>
  string(6) "bf-ecb"
  [112]=>
  string(6) "bf-ofb"
  [113]=>
  string(16) "camellia-128-cbc"
  [114]=>
  string(16) "camellia-128-cfb"
  [115]=>
  string(17) "camellia-128-cfb1"
  [116]=>
  string(17) "camellia-128-cfb8"
  [117]=>
  string(16) "camellia-128-ecb"
  [118]=>
  string(16) "camellia-128-ofb"
  [119]=>
  string(16) "camellia-192-cbc"
  [120]=>
  string(16) "camellia-192-cfb"
  [121]=>
  string(17) "camellia-192-cfb1"
  [122]=>
  string(17) "camellia-192-cfb8"
  [123]=>
  string(16) "camellia-192-ecb"
  [124]=>
  string(16) "camellia-192-ofb"
  [125]=>
  string(16) "camellia-256-cbc"
  [126]=>
  string(16) "camellia-256-cfb"
  [127]=>
  string(17) "camellia-256-cfb1"
  [128]=>
  string(17) "camellia-256-cfb8"
  [129]=>
  string(16) "camellia-256-ecb"
  [130]=>
  string(16) "camellia-256-ofb"
  [131]=>
  string(9) "cast5-cbc"
  [132]=>
  string(9) "cast5-cfb"
  [133]=>
  string(9) "cast5-ecb"
  [134]=>
  string(9) "cast5-ofb"
  [135]=>
  string(7) "des-cbc"
  [136]=>
  string(7) "des-cfb"
  [137]=>
  string(8) "des-cfb1"
  [138]=>
  string(8) "des-cfb8"
  [139]=>
  string(7) "des-ecb"
  [140]=>
  string(7) "des-ede"
  [141]=>
  string(11) "des-ede-cbc"
  [142]=>
  string(11) "des-ede-cfb"
  [143]=>
  string(11) "des-ede-ofb"
  [144]=>
  string(8) "des-ede3"
  [145]=>
  string(12) "des-ede3-cbc"
  [146]=>
  string(12) "des-ede3-cfb"
  [147]=>
  string(13) "des-ede3-cfb1"
  [148]=>
  string(13) "des-ede3-cfb8"
  [149]=>
  string(12) "des-ede3-ofb"
  [150]=>
  string(7) "des-ofb"
  [151]=>
  string(8) "desx-cbc"
  [152]=>
  string(13) "id-aes128-GCM"
  [153]=>
  string(13) "id-aes192-GCM"
  [154]=>
  string(13) "id-aes256-GCM"
  [155]=>
  string(8) "idea-cbc"
  [156]=>
  string(8) "idea-cfb"
  [157]=>
  string(8) "idea-ecb"
  [158]=>
  string(8) "idea-ofb"
  [159]=>
  string(10) "rc2-40-cbc"
  [160]=>
  string(10) "rc2-64-cbc"
  [161]=>
  string(7) "rc2-cbc"
  [162]=>
  string(7) "rc2-cfb"
  [163]=>
  string(7) "rc2-ecb"
  [164]=>
  string(7) "rc2-ofb"
  [165]=>
  string(3) "rc4"
  [166]=>
  string(6) "rc4-40"
  [167]=>
  string(12) "rc4-hmac-md5"
  [168]=>
  string(8) "seed-cbc"
  [169]=>
  string(8) "seed-cfb"
  [170]=>
  string(8) "seed-ecb"
  [171]=>
  string(8) "seed-ofb"
}
