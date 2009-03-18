--TEST--
Test DateTimeZone::listIdentifiers function : basic functionality 
--FILE--
<?php
/* Prototype  : array DateTimeZone::listIdentifiers ( void )
 * Description: Returns numerically index array with all timezone identifiers  
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_identifiers_list
 */

echo "*** Testing DateTimeZone::listIdentifiers() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("GMT");

var_dump( DateTimeZone::listIdentifiers() );

?>
===DONE===
--EXPECT--
*** Testing DateTimeZone::listIdentifiers() : basic functionality ***
array(402) {
  [0]=>
  string(14) "Africa/Abidjan"
  [1]=>
  string(12) "Africa/Accra"
  [2]=>
  string(18) "Africa/Addis_Ababa"
  [3]=>
  string(14) "Africa/Algiers"
  [4]=>
  string(13) "Africa/Asmara"
  [5]=>
  string(13) "Africa/Bamako"
  [6]=>
  string(13) "Africa/Bangui"
  [7]=>
  string(13) "Africa/Banjul"
  [8]=>
  string(13) "Africa/Bissau"
  [9]=>
  string(15) "Africa/Blantyre"
  [10]=>
  string(18) "Africa/Brazzaville"
  [11]=>
  string(16) "Africa/Bujumbura"
  [12]=>
  string(12) "Africa/Cairo"
  [13]=>
  string(17) "Africa/Casablanca"
  [14]=>
  string(12) "Africa/Ceuta"
  [15]=>
  string(14) "Africa/Conakry"
  [16]=>
  string(12) "Africa/Dakar"
  [17]=>
  string(20) "Africa/Dar_es_Salaam"
  [18]=>
  string(15) "Africa/Djibouti"
  [19]=>
  string(13) "Africa/Douala"
  [20]=>
  string(15) "Africa/El_Aaiun"
  [21]=>
  string(15) "Africa/Freetown"
  [22]=>
  string(15) "Africa/Gaborone"
  [23]=>
  string(13) "Africa/Harare"
  [24]=>
  string(19) "Africa/Johannesburg"
  [25]=>
  string(14) "Africa/Kampala"
  [26]=>
  string(15) "Africa/Khartoum"
  [27]=>
  string(13) "Africa/Kigali"
  [28]=>
  string(15) "Africa/Kinshasa"
  [29]=>
  string(12) "Africa/Lagos"
  [30]=>
  string(17) "Africa/Libreville"
  [31]=>
  string(11) "Africa/Lome"
  [32]=>
  string(13) "Africa/Luanda"
  [33]=>
  string(17) "Africa/Lubumbashi"
  [34]=>
  string(13) "Africa/Lusaka"
  [35]=>
  string(13) "Africa/Malabo"
  [36]=>
  string(13) "Africa/Maputo"
  [37]=>
  string(13) "Africa/Maseru"
  [38]=>
  string(14) "Africa/Mbabane"
  [39]=>
  string(16) "Africa/Mogadishu"
  [40]=>
  string(15) "Africa/Monrovia"
  [41]=>
  string(14) "Africa/Nairobi"
  [42]=>
  string(15) "Africa/Ndjamena"
  [43]=>
  string(13) "Africa/Niamey"
  [44]=>
  string(17) "Africa/Nouakchott"
  [45]=>
  string(18) "Africa/Ouagadougou"
  [46]=>
  string(17) "Africa/Porto-Novo"
  [47]=>
  string(15) "Africa/Sao_Tome"
  [48]=>
  string(14) "Africa/Tripoli"
  [49]=>
  string(12) "Africa/Tunis"
  [50]=>
  string(15) "Africa/Windhoek"
  [51]=>
  string(12) "America/Adak"
  [52]=>
  string(17) "America/Anchorage"
  [53]=>
  string(16) "America/Anguilla"
  [54]=>
  string(15) "America/Antigua"
  [55]=>
  string(17) "America/Araguaina"
  [56]=>
  string(30) "America/Argentina/Buenos_Aires"
  [57]=>
  string(27) "America/Argentina/Catamarca"
  [58]=>
  string(25) "America/Argentina/Cordoba"
  [59]=>
  string(23) "America/Argentina/Jujuy"
  [60]=>
  string(26) "America/Argentina/La_Rioja"
  [61]=>
  string(25) "America/Argentina/Mendoza"
  [62]=>
  string(30) "America/Argentina/Rio_Gallegos"
  [63]=>
  string(23) "America/Argentina/Salta"
  [64]=>
  string(26) "America/Argentina/San_Juan"
  [65]=>
  string(26) "America/Argentina/San_Luis"
  [66]=>
  string(25) "America/Argentina/Tucuman"
  [67]=>
  string(25) "America/Argentina/Ushuaia"
  [68]=>
  string(13) "America/Aruba"
  [69]=>
  string(16) "America/Asuncion"
  [70]=>
  string(16) "America/Atikokan"
  [71]=>
  string(13) "America/Bahia"
  [72]=>
  string(16) "America/Barbados"
  [73]=>
  string(13) "America/Belem"
  [74]=>
  string(14) "America/Belize"
  [75]=>
  string(20) "America/Blanc-Sablon"
  [76]=>
  string(17) "America/Boa_Vista"
  [77]=>
  string(14) "America/Bogota"
  [78]=>
  string(13) "America/Boise"
  [79]=>
  string(21) "America/Cambridge_Bay"
  [80]=>
  string(20) "America/Campo_Grande"
  [81]=>
  string(14) "America/Cancun"
  [82]=>
  string(15) "America/Caracas"
  [83]=>
  string(15) "America/Cayenne"
  [84]=>
  string(14) "America/Cayman"
  [85]=>
  string(15) "America/Chicago"
  [86]=>
  string(17) "America/Chihuahua"
  [87]=>
  string(18) "America/Costa_Rica"
  [88]=>
  string(14) "America/Cuiaba"
  [89]=>
  string(15) "America/Curacao"
  [90]=>
  string(20) "America/Danmarkshavn"
  [91]=>
  string(14) "America/Dawson"
  [92]=>
  string(20) "America/Dawson_Creek"
  [93]=>
  string(14) "America/Denver"
  [94]=>
  string(15) "America/Detroit"
  [95]=>
  string(16) "America/Dominica"
  [96]=>
  string(16) "America/Edmonton"
  [97]=>
  string(16) "America/Eirunepe"
  [98]=>
  string(19) "America/El_Salvador"
  [99]=>
  string(17) "America/Fortaleza"
  [100]=>
  string(17) "America/Glace_Bay"
  [101]=>
  string(15) "America/Godthab"
  [102]=>
  string(17) "America/Goose_Bay"
  [103]=>
  string(18) "America/Grand_Turk"
  [104]=>
  string(15) "America/Grenada"
  [105]=>
  string(18) "America/Guadeloupe"
  [106]=>
  string(17) "America/Guatemala"
  [107]=>
  string(17) "America/Guayaquil"
  [108]=>
  string(14) "America/Guyana"
  [109]=>
  string(15) "America/Halifax"
  [110]=>
  string(14) "America/Havana"
  [111]=>
  string(18) "America/Hermosillo"
  [112]=>
  string(28) "America/Indiana/Indianapolis"
  [113]=>
  string(20) "America/Indiana/Knox"
  [114]=>
  string(23) "America/Indiana/Marengo"
  [115]=>
  string(26) "America/Indiana/Petersburg"
  [116]=>
  string(25) "America/Indiana/Tell_City"
  [117]=>
  string(21) "America/Indiana/Vevay"
  [118]=>
  string(25) "America/Indiana/Vincennes"
  [119]=>
  string(23) "America/Indiana/Winamac"
  [120]=>
  string(14) "America/Inuvik"
  [121]=>
  string(15) "America/Iqaluit"
  [122]=>
  string(15) "America/Jamaica"
  [123]=>
  string(14) "America/Juneau"
  [124]=>
  string(27) "America/Kentucky/Louisville"
  [125]=>
  string(27) "America/Kentucky/Monticello"
  [126]=>
  string(14) "America/La_Paz"
  [127]=>
  string(12) "America/Lima"
  [128]=>
  string(19) "America/Los_Angeles"
  [129]=>
  string(14) "America/Maceio"
  [130]=>
  string(15) "America/Managua"
  [131]=>
  string(14) "America/Manaus"
  [132]=>
  string(15) "America/Marigot"
  [133]=>
  string(18) "America/Martinique"
  [134]=>
  string(16) "America/Mazatlan"
  [135]=>
  string(17) "America/Menominee"
  [136]=>
  string(14) "America/Merida"
  [137]=>
  string(19) "America/Mexico_City"
  [138]=>
  string(16) "America/Miquelon"
  [139]=>
  string(15) "America/Moncton"
  [140]=>
  string(17) "America/Monterrey"
  [141]=>
  string(18) "America/Montevideo"
  [142]=>
  string(16) "America/Montreal"
  [143]=>
  string(18) "America/Montserrat"
  [144]=>
  string(14) "America/Nassau"
  [145]=>
  string(16) "America/New_York"
  [146]=>
  string(15) "America/Nipigon"
  [147]=>
  string(12) "America/Nome"
  [148]=>
  string(15) "America/Noronha"
  [149]=>
  string(27) "America/North_Dakota/Center"
  [150]=>
  string(30) "America/North_Dakota/New_Salem"
  [151]=>
  string(14) "America/Panama"
  [152]=>
  string(19) "America/Pangnirtung"
  [153]=>
  string(18) "America/Paramaribo"
  [154]=>
  string(15) "America/Phoenix"
  [155]=>
  string(22) "America/Port-au-Prince"
  [156]=>
  string(21) "America/Port_of_Spain"
  [157]=>
  string(19) "America/Porto_Velho"
  [158]=>
  string(19) "America/Puerto_Rico"
  [159]=>
  string(19) "America/Rainy_River"
  [160]=>
  string(20) "America/Rankin_Inlet"
  [161]=>
  string(14) "America/Recife"
  [162]=>
  string(14) "America/Regina"
  [163]=>
  string(16) "America/Resolute"
  [164]=>
  string(18) "America/Rio_Branco"
  [165]=>
  string(16) "America/Santarem"
  [166]=>
  string(16) "America/Santiago"
  [167]=>
  string(21) "America/Santo_Domingo"
  [168]=>
  string(17) "America/Sao_Paulo"
  [169]=>
  string(20) "America/Scoresbysund"
  [170]=>
  string(16) "America/Shiprock"
  [171]=>
  string(21) "America/St_Barthelemy"
  [172]=>
  string(16) "America/St_Johns"
  [173]=>
  string(16) "America/St_Kitts"
  [174]=>
  string(16) "America/St_Lucia"
  [175]=>
  string(17) "America/St_Thomas"
  [176]=>
  string(18) "America/St_Vincent"
  [177]=>
  string(21) "America/Swift_Current"
  [178]=>
  string(19) "America/Tegucigalpa"
  [179]=>
  string(13) "America/Thule"
  [180]=>
  string(19) "America/Thunder_Bay"
  [181]=>
  string(15) "America/Tijuana"
  [182]=>
  string(15) "America/Toronto"
  [183]=>
  string(15) "America/Tortola"
  [184]=>
  string(17) "America/Vancouver"
  [185]=>
  string(18) "America/Whitehorse"
  [186]=>
  string(16) "America/Winnipeg"
  [187]=>
  string(15) "America/Yakutat"
  [188]=>
  string(19) "America/Yellowknife"
  [189]=>
  string(16) "Antarctica/Casey"
  [190]=>
  string(16) "Antarctica/Davis"
  [191]=>
  string(25) "Antarctica/DumontDUrville"
  [192]=>
  string(17) "Antarctica/Mawson"
  [193]=>
  string(18) "Antarctica/McMurdo"
  [194]=>
  string(17) "Antarctica/Palmer"
  [195]=>
  string(18) "Antarctica/Rothera"
  [196]=>
  string(21) "Antarctica/South_Pole"
  [197]=>
  string(16) "Antarctica/Syowa"
  [198]=>
  string(17) "Antarctica/Vostok"
  [199]=>
  string(19) "Arctic/Longyearbyen"
  [200]=>
  string(9) "Asia/Aden"
  [201]=>
  string(11) "Asia/Almaty"
  [202]=>
  string(10) "Asia/Amman"
  [203]=>
  string(11) "Asia/Anadyr"
  [204]=>
  string(10) "Asia/Aqtau"
  [205]=>
  string(11) "Asia/Aqtobe"
  [206]=>
  string(13) "Asia/Ashgabat"
  [207]=>
  string(12) "Asia/Baghdad"
  [208]=>
  string(12) "Asia/Bahrain"
  [209]=>
  string(9) "Asia/Baku"
  [210]=>
  string(12) "Asia/Bangkok"
  [211]=>
  string(11) "Asia/Beirut"
  [212]=>
  string(12) "Asia/Bishkek"
  [213]=>
  string(11) "Asia/Brunei"
  [214]=>
  string(15) "Asia/Choibalsan"
  [215]=>
  string(14) "Asia/Chongqing"
  [216]=>
  string(12) "Asia/Colombo"
  [217]=>
  string(13) "Asia/Damascus"
  [218]=>
  string(10) "Asia/Dhaka"
  [219]=>
  string(9) "Asia/Dili"
  [220]=>
  string(10) "Asia/Dubai"
  [221]=>
  string(13) "Asia/Dushanbe"
  [222]=>
  string(9) "Asia/Gaza"
  [223]=>
  string(11) "Asia/Harbin"
  [224]=>
  string(16) "Asia/Ho_Chi_Minh"
  [225]=>
  string(14) "Asia/Hong_Kong"
  [226]=>
  string(9) "Asia/Hovd"
  [227]=>
  string(12) "Asia/Irkutsk"
  [228]=>
  string(12) "Asia/Jakarta"
  [229]=>
  string(13) "Asia/Jayapura"
  [230]=>
  string(14) "Asia/Jerusalem"
  [231]=>
  string(10) "Asia/Kabul"
  [232]=>
  string(14) "Asia/Kamchatka"
  [233]=>
  string(12) "Asia/Karachi"
  [234]=>
  string(12) "Asia/Kashgar"
  [235]=>
  string(14) "Asia/Kathmandu"
  [236]=>
  string(12) "Asia/Kolkata"
  [237]=>
  string(16) "Asia/Krasnoyarsk"
  [238]=>
  string(17) "Asia/Kuala_Lumpur"
  [239]=>
  string(12) "Asia/Kuching"
  [240]=>
  string(11) "Asia/Kuwait"
  [241]=>
  string(10) "Asia/Macau"
  [242]=>
  string(12) "Asia/Magadan"
  [243]=>
  string(13) "Asia/Makassar"
  [244]=>
  string(11) "Asia/Manila"
  [245]=>
  string(11) "Asia/Muscat"
  [246]=>
  string(12) "Asia/Nicosia"
  [247]=>
  string(16) "Asia/Novosibirsk"
  [248]=>
  string(9) "Asia/Omsk"
  [249]=>
  string(9) "Asia/Oral"
  [250]=>
  string(15) "Asia/Phnom_Penh"
  [251]=>
  string(14) "Asia/Pontianak"
  [252]=>
  string(14) "Asia/Pyongyang"
  [253]=>
  string(10) "Asia/Qatar"
  [254]=>
  string(14) "Asia/Qyzylorda"
  [255]=>
  string(12) "Asia/Rangoon"
  [256]=>
  string(11) "Asia/Riyadh"
  [257]=>
  string(13) "Asia/Sakhalin"
  [258]=>
  string(14) "Asia/Samarkand"
  [259]=>
  string(10) "Asia/Seoul"
  [260]=>
  string(13) "Asia/Shanghai"
  [261]=>
  string(14) "Asia/Singapore"
  [262]=>
  string(11) "Asia/Taipei"
  [263]=>
  string(13) "Asia/Tashkent"
  [264]=>
  string(12) "Asia/Tbilisi"
  [265]=>
  string(11) "Asia/Tehran"
  [266]=>
  string(12) "Asia/Thimphu"
  [267]=>
  string(10) "Asia/Tokyo"
  [268]=>
  string(16) "Asia/Ulaanbaatar"
  [269]=>
  string(11) "Asia/Urumqi"
  [270]=>
  string(14) "Asia/Vientiane"
  [271]=>
  string(16) "Asia/Vladivostok"
  [272]=>
  string(12) "Asia/Yakutsk"
  [273]=>
  string(18) "Asia/Yekaterinburg"
  [274]=>
  string(12) "Asia/Yerevan"
  [275]=>
  string(15) "Atlantic/Azores"
  [276]=>
  string(16) "Atlantic/Bermuda"
  [277]=>
  string(15) "Atlantic/Canary"
  [278]=>
  string(19) "Atlantic/Cape_Verde"
  [279]=>
  string(14) "Atlantic/Faroe"
  [280]=>
  string(16) "Atlantic/Madeira"
  [281]=>
  string(18) "Atlantic/Reykjavik"
  [282]=>
  string(22) "Atlantic/South_Georgia"
  [283]=>
  string(18) "Atlantic/St_Helena"
  [284]=>
  string(16) "Atlantic/Stanley"
  [285]=>
  string(18) "Australia/Adelaide"
  [286]=>
  string(18) "Australia/Brisbane"
  [287]=>
  string(21) "Australia/Broken_Hill"
  [288]=>
  string(16) "Australia/Currie"
  [289]=>
  string(16) "Australia/Darwin"
  [290]=>
  string(15) "Australia/Eucla"
  [291]=>
  string(16) "Australia/Hobart"
  [292]=>
  string(18) "Australia/Lindeman"
  [293]=>
  string(19) "Australia/Lord_Howe"
  [294]=>
  string(19) "Australia/Melbourne"
  [295]=>
  string(15) "Australia/Perth"
  [296]=>
  string(16) "Australia/Sydney"
  [297]=>
  string(16) "Europe/Amsterdam"
  [298]=>
  string(14) "Europe/Andorra"
  [299]=>
  string(13) "Europe/Athens"
  [300]=>
  string(15) "Europe/Belgrade"
  [301]=>
  string(13) "Europe/Berlin"
  [302]=>
  string(17) "Europe/Bratislava"
  [303]=>
  string(15) "Europe/Brussels"
  [304]=>
  string(16) "Europe/Bucharest"
  [305]=>
  string(15) "Europe/Budapest"
  [306]=>
  string(15) "Europe/Chisinau"
  [307]=>
  string(17) "Europe/Copenhagen"
  [308]=>
  string(13) "Europe/Dublin"
  [309]=>
  string(16) "Europe/Gibraltar"
  [310]=>
  string(15) "Europe/Guernsey"
  [311]=>
  string(15) "Europe/Helsinki"
  [312]=>
  string(18) "Europe/Isle_of_Man"
  [313]=>
  string(15) "Europe/Istanbul"
  [314]=>
  string(13) "Europe/Jersey"
  [315]=>
  string(18) "Europe/Kaliningrad"
  [316]=>
  string(11) "Europe/Kiev"
  [317]=>
  string(13) "Europe/Lisbon"
  [318]=>
  string(16) "Europe/Ljubljana"
  [319]=>
  string(13) "Europe/London"
  [320]=>
  string(17) "Europe/Luxembourg"
  [321]=>
  string(13) "Europe/Madrid"
  [322]=>
  string(12) "Europe/Malta"
  [323]=>
  string(16) "Europe/Mariehamn"
  [324]=>
  string(12) "Europe/Minsk"
  [325]=>
  string(13) "Europe/Monaco"
  [326]=>
  string(13) "Europe/Moscow"
  [327]=>
  string(11) "Europe/Oslo"
  [328]=>
  string(12) "Europe/Paris"
  [329]=>
  string(16) "Europe/Podgorica"
  [330]=>
  string(13) "Europe/Prague"
  [331]=>
  string(11) "Europe/Riga"
  [332]=>
  string(11) "Europe/Rome"
  [333]=>
  string(13) "Europe/Samara"
  [334]=>
  string(17) "Europe/San_Marino"
  [335]=>
  string(15) "Europe/Sarajevo"
  [336]=>
  string(17) "Europe/Simferopol"
  [337]=>
  string(13) "Europe/Skopje"
  [338]=>
  string(12) "Europe/Sofia"
  [339]=>
  string(16) "Europe/Stockholm"
  [340]=>
  string(14) "Europe/Tallinn"
  [341]=>
  string(13) "Europe/Tirane"
  [342]=>
  string(15) "Europe/Uzhgorod"
  [343]=>
  string(12) "Europe/Vaduz"
  [344]=>
  string(14) "Europe/Vatican"
  [345]=>
  string(13) "Europe/Vienna"
  [346]=>
  string(14) "Europe/Vilnius"
  [347]=>
  string(16) "Europe/Volgograd"
  [348]=>
  string(13) "Europe/Warsaw"
  [349]=>
  string(13) "Europe/Zagreb"
  [350]=>
  string(17) "Europe/Zaporozhye"
  [351]=>
  string(13) "Europe/Zurich"
  [352]=>
  string(19) "Indian/Antananarivo"
  [353]=>
  string(13) "Indian/Chagos"
  [354]=>
  string(16) "Indian/Christmas"
  [355]=>
  string(12) "Indian/Cocos"
  [356]=>
  string(13) "Indian/Comoro"
  [357]=>
  string(16) "Indian/Kerguelen"
  [358]=>
  string(11) "Indian/Mahe"
  [359]=>
  string(15) "Indian/Maldives"
  [360]=>
  string(16) "Indian/Mauritius"
  [361]=>
  string(14) "Indian/Mayotte"
  [362]=>
  string(14) "Indian/Reunion"
  [363]=>
  string(12) "Pacific/Apia"
  [364]=>
  string(16) "Pacific/Auckland"
  [365]=>
  string(15) "Pacific/Chatham"
  [366]=>
  string(14) "Pacific/Easter"
  [367]=>
  string(13) "Pacific/Efate"
  [368]=>
  string(17) "Pacific/Enderbury"
  [369]=>
  string(15) "Pacific/Fakaofo"
  [370]=>
  string(12) "Pacific/Fiji"
  [371]=>
  string(16) "Pacific/Funafuti"
  [372]=>
  string(17) "Pacific/Galapagos"
  [373]=>
  string(15) "Pacific/Gambier"
  [374]=>
  string(19) "Pacific/Guadalcanal"
  [375]=>
  string(12) "Pacific/Guam"
  [376]=>
  string(16) "Pacific/Honolulu"
  [377]=>
  string(16) "Pacific/Johnston"
  [378]=>
  string(18) "Pacific/Kiritimati"
  [379]=>
  string(14) "Pacific/Kosrae"
  [380]=>
  string(17) "Pacific/Kwajalein"
  [381]=>
  string(14) "Pacific/Majuro"
  [382]=>
  string(17) "Pacific/Marquesas"
  [383]=>
  string(14) "Pacific/Midway"
  [384]=>
  string(13) "Pacific/Nauru"
  [385]=>
  string(12) "Pacific/Niue"
  [386]=>
  string(15) "Pacific/Norfolk"
  [387]=>
  string(14) "Pacific/Noumea"
  [388]=>
  string(17) "Pacific/Pago_Pago"
  [389]=>
  string(13) "Pacific/Palau"
  [390]=>
  string(16) "Pacific/Pitcairn"
  [391]=>
  string(14) "Pacific/Ponape"
  [392]=>
  string(20) "Pacific/Port_Moresby"
  [393]=>
  string(17) "Pacific/Rarotonga"
  [394]=>
  string(14) "Pacific/Saipan"
  [395]=>
  string(14) "Pacific/Tahiti"
  [396]=>
  string(14) "Pacific/Tarawa"
  [397]=>
  string(17) "Pacific/Tongatapu"
  [398]=>
  string(12) "Pacific/Truk"
  [399]=>
  string(12) "Pacific/Wake"
  [400]=>
  string(14) "Pacific/Wallis"
  [401]=>
  string(3) "UTC"
}
===DONE===