--TEST--
Test timezone_identifiers_list() function : basic functionality 
--FILE--
<?php
/* Prototype  : array timezone_identifiers_list  ( void )
 * Description: Returns numerically index array with all timezone identifiers  
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing timezone_identifiers_list() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("GMT");

var_dump (timezone_identifiers_list() );

?>
===DONE===
--EXPECTF--
*** Testing timezone_identifiers_list() : basic functionality ***
array(402) {
  [0]=>
  unicode(14) "Africa/Abidjan"
  [1]=>
  unicode(12) "Africa/Accra"
  [2]=>
  unicode(18) "Africa/Addis_Ababa"
  [3]=>
  unicode(14) "Africa/Algiers"
  [4]=>
  unicode(13) "Africa/Asmara"
  [5]=>
  unicode(13) "Africa/Bamako"
  [6]=>
  unicode(13) "Africa/Bangui"
  [7]=>
  unicode(13) "Africa/Banjul"
  [8]=>
  unicode(13) "Africa/Bissau"
  [9]=>
  unicode(15) "Africa/Blantyre"
  [10]=>
  unicode(18) "Africa/Brazzaville"
  [11]=>
  unicode(16) "Africa/Bujumbura"
  [12]=>
  unicode(12) "Africa/Cairo"
  [13]=>
  unicode(17) "Africa/Casablanca"
  [14]=>
  unicode(12) "Africa/Ceuta"
  [15]=>
  unicode(14) "Africa/Conakry"
  [16]=>
  unicode(12) "Africa/Dakar"
  [17]=>
  unicode(20) "Africa/Dar_es_Salaam"
  [18]=>
  unicode(15) "Africa/Djibouti"
  [19]=>
  unicode(13) "Africa/Douala"
  [20]=>
  unicode(15) "Africa/El_Aaiun"
  [21]=>
  unicode(15) "Africa/Freetown"
  [22]=>
  unicode(15) "Africa/Gaborone"
  [23]=>
  unicode(13) "Africa/Harare"
  [24]=>
  unicode(19) "Africa/Johannesburg"
  [25]=>
  unicode(14) "Africa/Kampala"
  [26]=>
  unicode(15) "Africa/Khartoum"
  [27]=>
  unicode(13) "Africa/Kigali"
  [28]=>
  unicode(15) "Africa/Kinshasa"
  [29]=>
  unicode(12) "Africa/Lagos"
  [30]=>
  unicode(17) "Africa/Libreville"
  [31]=>
  unicode(11) "Africa/Lome"
  [32]=>
  unicode(13) "Africa/Luanda"
  [33]=>
  unicode(17) "Africa/Lubumbashi"
  [34]=>
  unicode(13) "Africa/Lusaka"
  [35]=>
  unicode(13) "Africa/Malabo"
  [36]=>
  unicode(13) "Africa/Maputo"
  [37]=>
  unicode(13) "Africa/Maseru"
  [38]=>
  unicode(14) "Africa/Mbabane"
  [39]=>
  unicode(16) "Africa/Mogadishu"
  [40]=>
  unicode(15) "Africa/Monrovia"
  [41]=>
  unicode(14) "Africa/Nairobi"
  [42]=>
  unicode(15) "Africa/Ndjamena"
  [43]=>
  unicode(13) "Africa/Niamey"
  [44]=>
  unicode(17) "Africa/Nouakchott"
  [45]=>
  unicode(18) "Africa/Ouagadougou"
  [46]=>
  unicode(17) "Africa/Porto-Novo"
  [47]=>
  unicode(15) "Africa/Sao_Tome"
  [48]=>
  unicode(14) "Africa/Tripoli"
  [49]=>
  unicode(12) "Africa/Tunis"
  [50]=>
  unicode(15) "Africa/Windhoek"
  [51]=>
  unicode(12) "America/Adak"
  [52]=>
  unicode(17) "America/Anchorage"
  [53]=>
  unicode(16) "America/Anguilla"
  [54]=>
  unicode(15) "America/Antigua"
  [55]=>
  unicode(17) "America/Araguaina"
  [56]=>
  unicode(30) "America/Argentina/Buenos_Aires"
  [57]=>
  unicode(27) "America/Argentina/Catamarca"
  [58]=>
  unicode(25) "America/Argentina/Cordoba"
  [59]=>
  unicode(23) "America/Argentina/Jujuy"
  [60]=>
  unicode(26) "America/Argentina/La_Rioja"
  [61]=>
  unicode(25) "America/Argentina/Mendoza"
  [62]=>
  unicode(30) "America/Argentina/Rio_Gallegos"
  [63]=>
  unicode(23) "America/Argentina/Salta"
  [64]=>
  unicode(26) "America/Argentina/San_Juan"
  [65]=>
  unicode(26) "America/Argentina/San_Luis"
  [66]=>
  unicode(25) "America/Argentina/Tucuman"
  [67]=>
  unicode(25) "America/Argentina/Ushuaia"
  [68]=>
  unicode(13) "America/Aruba"
  [69]=>
  unicode(16) "America/Asuncion"
  [70]=>
  unicode(16) "America/Atikokan"
  [71]=>
  unicode(13) "America/Bahia"
  [72]=>
  unicode(16) "America/Barbados"
  [73]=>
  unicode(13) "America/Belem"
  [74]=>
  unicode(14) "America/Belize"
  [75]=>
  unicode(20) "America/Blanc-Sablon"
  [76]=>
  unicode(17) "America/Boa_Vista"
  [77]=>
  unicode(14) "America/Bogota"
  [78]=>
  unicode(13) "America/Boise"
  [79]=>
  unicode(21) "America/Cambridge_Bay"
  [80]=>
  unicode(20) "America/Campo_Grande"
  [81]=>
  unicode(14) "America/Cancun"
  [82]=>
  unicode(15) "America/Caracas"
  [83]=>
  unicode(15) "America/Cayenne"
  [84]=>
  unicode(14) "America/Cayman"
  [85]=>
  unicode(15) "America/Chicago"
  [86]=>
  unicode(17) "America/Chihuahua"
  [87]=>
  unicode(18) "America/Costa_Rica"
  [88]=>
  unicode(14) "America/Cuiaba"
  [89]=>
  unicode(15) "America/Curacao"
  [90]=>
  unicode(20) "America/Danmarkshavn"
  [91]=>
  unicode(14) "America/Dawson"
  [92]=>
  unicode(20) "America/Dawson_Creek"
  [93]=>
  unicode(14) "America/Denver"
  [94]=>
  unicode(15) "America/Detroit"
  [95]=>
  unicode(16) "America/Dominica"
  [96]=>
  unicode(16) "America/Edmonton"
  [97]=>
  unicode(16) "America/Eirunepe"
  [98]=>
  unicode(19) "America/El_Salvador"
  [99]=>
  unicode(17) "America/Fortaleza"
  [100]=>
  unicode(17) "America/Glace_Bay"
  [101]=>
  unicode(15) "America/Godthab"
  [102]=>
  unicode(17) "America/Goose_Bay"
  [103]=>
  unicode(18) "America/Grand_Turk"
  [104]=>
  unicode(15) "America/Grenada"
  [105]=>
  unicode(18) "America/Guadeloupe"
  [106]=>
  unicode(17) "America/Guatemala"
  [107]=>
  unicode(17) "America/Guayaquil"
  [108]=>
  unicode(14) "America/Guyana"
  [109]=>
  unicode(15) "America/Halifax"
  [110]=>
  unicode(14) "America/Havana"
  [111]=>
  unicode(18) "America/Hermosillo"
  [112]=>
  unicode(28) "America/Indiana/Indianapolis"
  [113]=>
  unicode(20) "America/Indiana/Knox"
  [114]=>
  unicode(23) "America/Indiana/Marengo"
  [115]=>
  unicode(26) "America/Indiana/Petersburg"
  [116]=>
  unicode(25) "America/Indiana/Tell_City"
  [117]=>
  unicode(21) "America/Indiana/Vevay"
  [118]=>
  unicode(25) "America/Indiana/Vincennes"
  [119]=>
  unicode(23) "America/Indiana/Winamac"
  [120]=>
  unicode(14) "America/Inuvik"
  [121]=>
  unicode(15) "America/Iqaluit"
  [122]=>
  unicode(15) "America/Jamaica"
  [123]=>
  unicode(14) "America/Juneau"
  [124]=>
  unicode(27) "America/Kentucky/Louisville"
  [125]=>
  unicode(27) "America/Kentucky/Monticello"
  [126]=>
  unicode(14) "America/La_Paz"
  [127]=>
  unicode(12) "America/Lima"
  [128]=>
  unicode(19) "America/Los_Angeles"
  [129]=>
  unicode(14) "America/Maceio"
  [130]=>
  unicode(15) "America/Managua"
  [131]=>
  unicode(14) "America/Manaus"
  [132]=>
  unicode(15) "America/Marigot"
  [133]=>
  unicode(18) "America/Martinique"
  [134]=>
  unicode(16) "America/Mazatlan"
  [135]=>
  unicode(17) "America/Menominee"
  [136]=>
  unicode(14) "America/Merida"
  [137]=>
  unicode(19) "America/Mexico_City"
  [138]=>
  unicode(16) "America/Miquelon"
  [139]=>
  unicode(15) "America/Moncton"
  [140]=>
  unicode(17) "America/Monterrey"
  [141]=>
  unicode(18) "America/Montevideo"
  [142]=>
  unicode(16) "America/Montreal"
  [143]=>
  unicode(18) "America/Montserrat"
  [144]=>
  unicode(14) "America/Nassau"
  [145]=>
  unicode(16) "America/New_York"
  [146]=>
  unicode(15) "America/Nipigon"
  [147]=>
  unicode(12) "America/Nome"
  [148]=>
  unicode(15) "America/Noronha"
  [149]=>
  unicode(27) "America/North_Dakota/Center"
  [150]=>
  unicode(30) "America/North_Dakota/New_Salem"
  [151]=>
  unicode(14) "America/Panama"
  [152]=>
  unicode(19) "America/Pangnirtung"
  [153]=>
  unicode(18) "America/Paramaribo"
  [154]=>
  unicode(15) "America/Phoenix"
  [155]=>
  unicode(22) "America/Port-au-Prince"
  [156]=>
  unicode(21) "America/Port_of_Spain"
  [157]=>
  unicode(19) "America/Porto_Velho"
  [158]=>
  unicode(19) "America/Puerto_Rico"
  [159]=>
  unicode(19) "America/Rainy_River"
  [160]=>
  unicode(20) "America/Rankin_Inlet"
  [161]=>
  unicode(14) "America/Recife"
  [162]=>
  unicode(14) "America/Regina"
  [163]=>
  unicode(16) "America/Resolute"
  [164]=>
  unicode(18) "America/Rio_Branco"
  [165]=>
  unicode(16) "America/Santarem"
  [166]=>
  unicode(16) "America/Santiago"
  [167]=>
  unicode(21) "America/Santo_Domingo"
  [168]=>
  unicode(17) "America/Sao_Paulo"
  [169]=>
  unicode(20) "America/Scoresbysund"
  [170]=>
  unicode(16) "America/Shiprock"
  [171]=>
  unicode(21) "America/St_Barthelemy"
  [172]=>
  unicode(16) "America/St_Johns"
  [173]=>
  unicode(16) "America/St_Kitts"
  [174]=>
  unicode(16) "America/St_Lucia"
  [175]=>
  unicode(17) "America/St_Thomas"
  [176]=>
  unicode(18) "America/St_Vincent"
  [177]=>
  unicode(21) "America/Swift_Current"
  [178]=>
  unicode(19) "America/Tegucigalpa"
  [179]=>
  unicode(13) "America/Thule"
  [180]=>
  unicode(19) "America/Thunder_Bay"
  [181]=>
  unicode(15) "America/Tijuana"
  [182]=>
  unicode(15) "America/Toronto"
  [183]=>
  unicode(15) "America/Tortola"
  [184]=>
  unicode(17) "America/Vancouver"
  [185]=>
  unicode(18) "America/Whitehorse"
  [186]=>
  unicode(16) "America/Winnipeg"
  [187]=>
  unicode(15) "America/Yakutat"
  [188]=>
  unicode(19) "America/Yellowknife"
  [189]=>
  unicode(16) "Antarctica/Casey"
  [190]=>
  unicode(16) "Antarctica/Davis"
  [191]=>
  unicode(25) "Antarctica/DumontDUrville"
  [192]=>
  unicode(17) "Antarctica/Mawson"
  [193]=>
  unicode(18) "Antarctica/McMurdo"
  [194]=>
  unicode(17) "Antarctica/Palmer"
  [195]=>
  unicode(18) "Antarctica/Rothera"
  [196]=>
  unicode(21) "Antarctica/South_Pole"
  [197]=>
  unicode(16) "Antarctica/Syowa"
  [198]=>
  unicode(17) "Antarctica/Vostok"
  [199]=>
  unicode(19) "Arctic/Longyearbyen"
  [200]=>
  unicode(9) "Asia/Aden"
  [201]=>
  unicode(11) "Asia/Almaty"
  [202]=>
  unicode(10) "Asia/Amman"
  [203]=>
  unicode(11) "Asia/Anadyr"
  [204]=>
  unicode(10) "Asia/Aqtau"
  [205]=>
  unicode(11) "Asia/Aqtobe"
  [206]=>
  unicode(13) "Asia/Ashgabat"
  [207]=>
  unicode(12) "Asia/Baghdad"
  [208]=>
  unicode(12) "Asia/Bahrain"
  [209]=>
  unicode(9) "Asia/Baku"
  [210]=>
  unicode(12) "Asia/Bangkok"
  [211]=>
  unicode(11) "Asia/Beirut"
  [212]=>
  unicode(12) "Asia/Bishkek"
  [213]=>
  unicode(11) "Asia/Brunei"
  [214]=>
  unicode(15) "Asia/Choibalsan"
  [215]=>
  unicode(14) "Asia/Chongqing"
  [216]=>
  unicode(12) "Asia/Colombo"
  [217]=>
  unicode(13) "Asia/Damascus"
  [218]=>
  unicode(10) "Asia/Dhaka"
  [219]=>
  unicode(9) "Asia/Dili"
  [220]=>
  unicode(10) "Asia/Dubai"
  [221]=>
  unicode(13) "Asia/Dushanbe"
  [222]=>
  unicode(9) "Asia/Gaza"
  [223]=>
  unicode(11) "Asia/Harbin"
  [224]=>
  unicode(16) "Asia/Ho_Chi_Minh"
  [225]=>
  unicode(14) "Asia/Hong_Kong"
  [226]=>
  unicode(9) "Asia/Hovd"
  [227]=>
  unicode(12) "Asia/Irkutsk"
  [228]=>
  unicode(12) "Asia/Jakarta"
  [229]=>
  unicode(13) "Asia/Jayapura"
  [230]=>
  unicode(14) "Asia/Jerusalem"
  [231]=>
  unicode(10) "Asia/Kabul"
  [232]=>
  unicode(14) "Asia/Kamchatka"
  [233]=>
  unicode(12) "Asia/Karachi"
  [234]=>
  unicode(12) "Asia/Kashgar"
  [235]=>
  unicode(14) "Asia/Kathmandu"
  [236]=>
  unicode(12) "Asia/Kolkata"
  [237]=>
  unicode(16) "Asia/Krasnoyarsk"
  [238]=>
  unicode(17) "Asia/Kuala_Lumpur"
  [239]=>
  unicode(12) "Asia/Kuching"
  [240]=>
  unicode(11) "Asia/Kuwait"
  [241]=>
  unicode(10) "Asia/Macau"
  [242]=>
  unicode(12) "Asia/Magadan"
  [243]=>
  unicode(13) "Asia/Makassar"
  [244]=>
  unicode(11) "Asia/Manila"
  [245]=>
  unicode(11) "Asia/Muscat"
  [246]=>
  unicode(12) "Asia/Nicosia"
  [247]=>
  unicode(16) "Asia/Novosibirsk"
  [248]=>
  unicode(9) "Asia/Omsk"
  [249]=>
  unicode(9) "Asia/Oral"
  [250]=>
  unicode(15) "Asia/Phnom_Penh"
  [251]=>
  unicode(14) "Asia/Pontianak"
  [252]=>
  unicode(14) "Asia/Pyongyang"
  [253]=>
  unicode(10) "Asia/Qatar"
  [254]=>
  unicode(14) "Asia/Qyzylorda"
  [255]=>
  unicode(12) "Asia/Rangoon"
  [256]=>
  unicode(11) "Asia/Riyadh"
  [257]=>
  unicode(13) "Asia/Sakhalin"
  [258]=>
  unicode(14) "Asia/Samarkand"
  [259]=>
  unicode(10) "Asia/Seoul"
  [260]=>
  unicode(13) "Asia/Shanghai"
  [261]=>
  unicode(14) "Asia/Singapore"
  [262]=>
  unicode(11) "Asia/Taipei"
  [263]=>
  unicode(13) "Asia/Tashkent"
  [264]=>
  unicode(12) "Asia/Tbilisi"
  [265]=>
  unicode(11) "Asia/Tehran"
  [266]=>
  unicode(12) "Asia/Thimphu"
  [267]=>
  unicode(10) "Asia/Tokyo"
  [268]=>
  unicode(16) "Asia/Ulaanbaatar"
  [269]=>
  unicode(11) "Asia/Urumqi"
  [270]=>
  unicode(14) "Asia/Vientiane"
  [271]=>
  unicode(16) "Asia/Vladivostok"
  [272]=>
  unicode(12) "Asia/Yakutsk"
  [273]=>
  unicode(18) "Asia/Yekaterinburg"
  [274]=>
  unicode(12) "Asia/Yerevan"
  [275]=>
  unicode(15) "Atlantic/Azores"
  [276]=>
  unicode(16) "Atlantic/Bermuda"
  [277]=>
  unicode(15) "Atlantic/Canary"
  [278]=>
  unicode(19) "Atlantic/Cape_Verde"
  [279]=>
  unicode(14) "Atlantic/Faroe"
  [280]=>
  unicode(16) "Atlantic/Madeira"
  [281]=>
  unicode(18) "Atlantic/Reykjavik"
  [282]=>
  unicode(22) "Atlantic/South_Georgia"
  [283]=>
  unicode(18) "Atlantic/St_Helena"
  [284]=>
  unicode(16) "Atlantic/Stanley"
  [285]=>
  unicode(18) "Australia/Adelaide"
  [286]=>
  unicode(18) "Australia/Brisbane"
  [287]=>
  unicode(21) "Australia/Broken_Hill"
  [288]=>
  unicode(16) "Australia/Currie"
  [289]=>
  unicode(16) "Australia/Darwin"
  [290]=>
  unicode(15) "Australia/Eucla"
  [291]=>
  unicode(16) "Australia/Hobart"
  [292]=>
  unicode(18) "Australia/Lindeman"
  [293]=>
  unicode(19) "Australia/Lord_Howe"
  [294]=>
  unicode(19) "Australia/Melbourne"
  [295]=>
  unicode(15) "Australia/Perth"
  [296]=>
  unicode(16) "Australia/Sydney"
  [297]=>
  unicode(16) "Europe/Amsterdam"
  [298]=>
  unicode(14) "Europe/Andorra"
  [299]=>
  unicode(13) "Europe/Athens"
  [300]=>
  unicode(15) "Europe/Belgrade"
  [301]=>
  unicode(13) "Europe/Berlin"
  [302]=>
  unicode(17) "Europe/Bratislava"
  [303]=>
  unicode(15) "Europe/Brussels"
  [304]=>
  unicode(16) "Europe/Bucharest"
  [305]=>
  unicode(15) "Europe/Budapest"
  [306]=>
  unicode(15) "Europe/Chisinau"
  [307]=>
  unicode(17) "Europe/Copenhagen"
  [308]=>
  unicode(13) "Europe/Dublin"
  [309]=>
  unicode(16) "Europe/Gibraltar"
  [310]=>
  unicode(15) "Europe/Guernsey"
  [311]=>
  unicode(15) "Europe/Helsinki"
  [312]=>
  unicode(18) "Europe/Isle_of_Man"
  [313]=>
  unicode(15) "Europe/Istanbul"
  [314]=>
  unicode(13) "Europe/Jersey"
  [315]=>
  unicode(18) "Europe/Kaliningrad"
  [316]=>
  unicode(11) "Europe/Kiev"
  [317]=>
  unicode(13) "Europe/Lisbon"
  [318]=>
  unicode(16) "Europe/Ljubljana"
  [319]=>
  unicode(13) "Europe/London"
  [320]=>
  unicode(17) "Europe/Luxembourg"
  [321]=>
  unicode(13) "Europe/Madrid"
  [322]=>
  unicode(12) "Europe/Malta"
  [323]=>
  unicode(16) "Europe/Mariehamn"
  [324]=>
  unicode(12) "Europe/Minsk"
  [325]=>
  unicode(13) "Europe/Monaco"
  [326]=>
  unicode(13) "Europe/Moscow"
  [327]=>
  unicode(11) "Europe/Oslo"
  [328]=>
  unicode(12) "Europe/Paris"
  [329]=>
  unicode(16) "Europe/Podgorica"
  [330]=>
  unicode(13) "Europe/Prague"
  [331]=>
  unicode(11) "Europe/Riga"
  [332]=>
  unicode(11) "Europe/Rome"
  [333]=>
  unicode(13) "Europe/Samara"
  [334]=>
  unicode(17) "Europe/San_Marino"
  [335]=>
  unicode(15) "Europe/Sarajevo"
  [336]=>
  unicode(17) "Europe/Simferopol"
  [337]=>
  unicode(13) "Europe/Skopje"
  [338]=>
  unicode(12) "Europe/Sofia"
  [339]=>
  unicode(16) "Europe/Stockholm"
  [340]=>
  unicode(14) "Europe/Tallinn"
  [341]=>
  unicode(13) "Europe/Tirane"
  [342]=>
  unicode(15) "Europe/Uzhgorod"
  [343]=>
  unicode(12) "Europe/Vaduz"
  [344]=>
  unicode(14) "Europe/Vatican"
  [345]=>
  unicode(13) "Europe/Vienna"
  [346]=>
  unicode(14) "Europe/Vilnius"
  [347]=>
  unicode(16) "Europe/Volgograd"
  [348]=>
  unicode(13) "Europe/Warsaw"
  [349]=>
  unicode(13) "Europe/Zagreb"
  [350]=>
  unicode(17) "Europe/Zaporozhye"
  [351]=>
  unicode(13) "Europe/Zurich"
  [352]=>
  unicode(19) "Indian/Antananarivo"
  [353]=>
  unicode(13) "Indian/Chagos"
  [354]=>
  unicode(16) "Indian/Christmas"
  [355]=>
  unicode(12) "Indian/Cocos"
  [356]=>
  unicode(13) "Indian/Comoro"
  [357]=>
  unicode(16) "Indian/Kerguelen"
  [358]=>
  unicode(11) "Indian/Mahe"
  [359]=>
  unicode(15) "Indian/Maldives"
  [360]=>
  unicode(16) "Indian/Mauritius"
  [361]=>
  unicode(14) "Indian/Mayotte"
  [362]=>
  unicode(14) "Indian/Reunion"
  [363]=>
  unicode(12) "Pacific/Apia"
  [364]=>
  unicode(16) "Pacific/Auckland"
  [365]=>
  unicode(15) "Pacific/Chatham"
  [366]=>
  unicode(14) "Pacific/Easter"
  [367]=>
  unicode(13) "Pacific/Efate"
  [368]=>
  unicode(17) "Pacific/Enderbury"
  [369]=>
  unicode(15) "Pacific/Fakaofo"
  [370]=>
  unicode(12) "Pacific/Fiji"
  [371]=>
  unicode(16) "Pacific/Funafuti"
  [372]=>
  unicode(17) "Pacific/Galapagos"
  [373]=>
  unicode(15) "Pacific/Gambier"
  [374]=>
  unicode(19) "Pacific/Guadalcanal"
  [375]=>
  unicode(12) "Pacific/Guam"
  [376]=>
  unicode(16) "Pacific/Honolulu"
  [377]=>
  unicode(16) "Pacific/Johnston"
  [378]=>
  unicode(18) "Pacific/Kiritimati"
  [379]=>
  unicode(14) "Pacific/Kosrae"
  [380]=>
  unicode(17) "Pacific/Kwajalein"
  [381]=>
  unicode(14) "Pacific/Majuro"
  [382]=>
  unicode(17) "Pacific/Marquesas"
  [383]=>
  unicode(14) "Pacific/Midway"
  [384]=>
  unicode(13) "Pacific/Nauru"
  [385]=>
  unicode(12) "Pacific/Niue"
  [386]=>
  unicode(15) "Pacific/Norfolk"
  [387]=>
  unicode(14) "Pacific/Noumea"
  [388]=>
  unicode(17) "Pacific/Pago_Pago"
  [389]=>
  unicode(13) "Pacific/Palau"
  [390]=>
  unicode(16) "Pacific/Pitcairn"
  [391]=>
  unicode(14) "Pacific/Ponape"
  [392]=>
  unicode(20) "Pacific/Port_Moresby"
  [393]=>
  unicode(17) "Pacific/Rarotonga"
  [394]=>
  unicode(14) "Pacific/Saipan"
  [395]=>
  unicode(14) "Pacific/Tahiti"
  [396]=>
  unicode(14) "Pacific/Tarawa"
  [397]=>
  unicode(17) "Pacific/Tongatapu"
  [398]=>
  unicode(12) "Pacific/Truk"
  [399]=>
  unicode(12) "Pacific/Wake"
  [400]=>
  unicode(14) "Pacific/Wallis"
  [401]=>
  unicode(3) "UTC"
}
===DONE===
