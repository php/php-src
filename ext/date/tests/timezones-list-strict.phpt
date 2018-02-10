--TEST--
timezone_identifiers_list() [strict_types]
--FILE--
<?php
declare(strict_types=1);
var_dump(\DateTimeZone::listIdentifiers(\DateTimeZone::ALL, null));
?>
--EXPECT--
array(425) {
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
  string(11) "Africa/Juba"
  [26]=>
  string(14) "Africa/Kampala"
  [27]=>
  string(15) "Africa/Khartoum"
  [28]=>
  string(13) "Africa/Kigali"
  [29]=>
  string(15) "Africa/Kinshasa"
  [30]=>
  string(12) "Africa/Lagos"
  [31]=>
  string(17) "Africa/Libreville"
  [32]=>
  string(11) "Africa/Lome"
  [33]=>
  string(13) "Africa/Luanda"
  [34]=>
  string(17) "Africa/Lubumbashi"
  [35]=>
  string(13) "Africa/Lusaka"
  [36]=>
  string(13) "Africa/Malabo"
  [37]=>
  string(13) "Africa/Maputo"
  [38]=>
  string(13) "Africa/Maseru"
  [39]=>
  string(14) "Africa/Mbabane"
  [40]=>
  string(16) "Africa/Mogadishu"
  [41]=>
  string(15) "Africa/Monrovia"
  [42]=>
  string(14) "Africa/Nairobi"
  [43]=>
  string(15) "Africa/Ndjamena"
  [44]=>
  string(13) "Africa/Niamey"
  [45]=>
  string(17) "Africa/Nouakchott"
  [46]=>
  string(18) "Africa/Ouagadougou"
  [47]=>
  string(17) "Africa/Porto-Novo"
  [48]=>
  string(15) "Africa/Sao_Tome"
  [49]=>
  string(14) "Africa/Tripoli"
  [50]=>
  string(12) "Africa/Tunis"
  [51]=>
  string(15) "Africa/Windhoek"
  [52]=>
  string(12) "America/Adak"
  [53]=>
  string(17) "America/Anchorage"
  [54]=>
  string(16) "America/Anguilla"
  [55]=>
  string(15) "America/Antigua"
  [56]=>
  string(17) "America/Araguaina"
  [57]=>
  string(30) "America/Argentina/Buenos_Aires"
  [58]=>
  string(27) "America/Argentina/Catamarca"
  [59]=>
  string(25) "America/Argentina/Cordoba"
  [60]=>
  string(23) "America/Argentina/Jujuy"
  [61]=>
  string(26) "America/Argentina/La_Rioja"
  [62]=>
  string(25) "America/Argentina/Mendoza"
  [63]=>
  string(30) "America/Argentina/Rio_Gallegos"
  [64]=>
  string(23) "America/Argentina/Salta"
  [65]=>
  string(26) "America/Argentina/San_Juan"
  [66]=>
  string(26) "America/Argentina/San_Luis"
  [67]=>
  string(25) "America/Argentina/Tucuman"
  [68]=>
  string(25) "America/Argentina/Ushuaia"
  [69]=>
  string(13) "America/Aruba"
  [70]=>
  string(16) "America/Asuncion"
  [71]=>
  string(16) "America/Atikokan"
  [72]=>
  string(13) "America/Bahia"
  [73]=>
  string(22) "America/Bahia_Banderas"
  [74]=>
  string(16) "America/Barbados"
  [75]=>
  string(13) "America/Belem"
  [76]=>
  string(14) "America/Belize"
  [77]=>
  string(20) "America/Blanc-Sablon"
  [78]=>
  string(17) "America/Boa_Vista"
  [79]=>
  string(14) "America/Bogota"
  [80]=>
  string(13) "America/Boise"
  [81]=>
  string(21) "America/Cambridge_Bay"
  [82]=>
  string(20) "America/Campo_Grande"
  [83]=>
  string(14) "America/Cancun"
  [84]=>
  string(15) "America/Caracas"
  [85]=>
  string(15) "America/Cayenne"
  [86]=>
  string(14) "America/Cayman"
  [87]=>
  string(15) "America/Chicago"
  [88]=>
  string(17) "America/Chihuahua"
  [89]=>
  string(18) "America/Costa_Rica"
  [90]=>
  string(15) "America/Creston"
  [91]=>
  string(14) "America/Cuiaba"
  [92]=>
  string(15) "America/Curacao"
  [93]=>
  string(20) "America/Danmarkshavn"
  [94]=>
  string(14) "America/Dawson"
  [95]=>
  string(20) "America/Dawson_Creek"
  [96]=>
  string(14) "America/Denver"
  [97]=>
  string(15) "America/Detroit"
  [98]=>
  string(16) "America/Dominica"
  [99]=>
  string(16) "America/Edmonton"
  [100]=>
  string(16) "America/Eirunepe"
  [101]=>
  string(19) "America/El_Salvador"
  [102]=>
  string(19) "America/Fort_Nelson"
  [103]=>
  string(17) "America/Fortaleza"
  [104]=>
  string(17) "America/Glace_Bay"
  [105]=>
  string(15) "America/Godthab"
  [106]=>
  string(17) "America/Goose_Bay"
  [107]=>
  string(18) "America/Grand_Turk"
  [108]=>
  string(15) "America/Grenada"
  [109]=>
  string(18) "America/Guadeloupe"
  [110]=>
  string(17) "America/Guatemala"
  [111]=>
  string(17) "America/Guayaquil"
  [112]=>
  string(14) "America/Guyana"
  [113]=>
  string(15) "America/Halifax"
  [114]=>
  string(14) "America/Havana"
  [115]=>
  string(18) "America/Hermosillo"
  [116]=>
  string(28) "America/Indiana/Indianapolis"
  [117]=>
  string(20) "America/Indiana/Knox"
  [118]=>
  string(23) "America/Indiana/Marengo"
  [119]=>
  string(26) "America/Indiana/Petersburg"
  [120]=>
  string(25) "America/Indiana/Tell_City"
  [121]=>
  string(21) "America/Indiana/Vevay"
  [122]=>
  string(25) "America/Indiana/Vincennes"
  [123]=>
  string(23) "America/Indiana/Winamac"
  [124]=>
  string(14) "America/Inuvik"
  [125]=>
  string(15) "America/Iqaluit"
  [126]=>
  string(15) "America/Jamaica"
  [127]=>
  string(14) "America/Juneau"
  [128]=>
  string(27) "America/Kentucky/Louisville"
  [129]=>
  string(27) "America/Kentucky/Monticello"
  [130]=>
  string(18) "America/Kralendijk"
  [131]=>
  string(14) "America/La_Paz"
  [132]=>
  string(12) "America/Lima"
  [133]=>
  string(19) "America/Los_Angeles"
  [134]=>
  string(21) "America/Lower_Princes"
  [135]=>
  string(14) "America/Maceio"
  [136]=>
  string(15) "America/Managua"
  [137]=>
  string(14) "America/Manaus"
  [138]=>
  string(15) "America/Marigot"
  [139]=>
  string(18) "America/Martinique"
  [140]=>
  string(17) "America/Matamoros"
  [141]=>
  string(16) "America/Mazatlan"
  [142]=>
  string(17) "America/Menominee"
  [143]=>
  string(14) "America/Merida"
  [144]=>
  string(18) "America/Metlakatla"
  [145]=>
  string(19) "America/Mexico_City"
  [146]=>
  string(16) "America/Miquelon"
  [147]=>
  string(15) "America/Moncton"
  [148]=>
  string(17) "America/Monterrey"
  [149]=>
  string(18) "America/Montevideo"
  [150]=>
  string(18) "America/Montserrat"
  [151]=>
  string(14) "America/Nassau"
  [152]=>
  string(16) "America/New_York"
  [153]=>
  string(15) "America/Nipigon"
  [154]=>
  string(12) "America/Nome"
  [155]=>
  string(15) "America/Noronha"
  [156]=>
  string(27) "America/North_Dakota/Beulah"
  [157]=>
  string(27) "America/North_Dakota/Center"
  [158]=>
  string(30) "America/North_Dakota/New_Salem"
  [159]=>
  string(15) "America/Ojinaga"
  [160]=>
  string(14) "America/Panama"
  [161]=>
  string(19) "America/Pangnirtung"
  [162]=>
  string(18) "America/Paramaribo"
  [163]=>
  string(15) "America/Phoenix"
  [164]=>
  string(22) "America/Port-au-Prince"
  [165]=>
  string(21) "America/Port_of_Spain"
  [166]=>
  string(19) "America/Porto_Velho"
  [167]=>
  string(19) "America/Puerto_Rico"
  [168]=>
  string(20) "America/Punta_Arenas"
  [169]=>
  string(19) "America/Rainy_River"
  [170]=>
  string(20) "America/Rankin_Inlet"
  [171]=>
  string(14) "America/Recife"
  [172]=>
  string(14) "America/Regina"
  [173]=>
  string(16) "America/Resolute"
  [174]=>
  string(18) "America/Rio_Branco"
  [175]=>
  string(16) "America/Santarem"
  [176]=>
  string(16) "America/Santiago"
  [177]=>
  string(21) "America/Santo_Domingo"
  [178]=>
  string(17) "America/Sao_Paulo"
  [179]=>
  string(20) "America/Scoresbysund"
  [180]=>
  string(13) "America/Sitka"
  [181]=>
  string(21) "America/St_Barthelemy"
  [182]=>
  string(16) "America/St_Johns"
  [183]=>
  string(16) "America/St_Kitts"
  [184]=>
  string(16) "America/St_Lucia"
  [185]=>
  string(17) "America/St_Thomas"
  [186]=>
  string(18) "America/St_Vincent"
  [187]=>
  string(21) "America/Swift_Current"
  [188]=>
  string(19) "America/Tegucigalpa"
  [189]=>
  string(13) "America/Thule"
  [190]=>
  string(19) "America/Thunder_Bay"
  [191]=>
  string(15) "America/Tijuana"
  [192]=>
  string(15) "America/Toronto"
  [193]=>
  string(15) "America/Tortola"
  [194]=>
  string(17) "America/Vancouver"
  [195]=>
  string(18) "America/Whitehorse"
  [196]=>
  string(16) "America/Winnipeg"
  [197]=>
  string(15) "America/Yakutat"
  [198]=>
  string(19) "America/Yellowknife"
  [199]=>
  string(16) "Antarctica/Casey"
  [200]=>
  string(16) "Antarctica/Davis"
  [201]=>
  string(25) "Antarctica/DumontDUrville"
  [202]=>
  string(20) "Antarctica/Macquarie"
  [203]=>
  string(17) "Antarctica/Mawson"
  [204]=>
  string(18) "Antarctica/McMurdo"
  [205]=>
  string(17) "Antarctica/Palmer"
  [206]=>
  string(18) "Antarctica/Rothera"
  [207]=>
  string(16) "Antarctica/Syowa"
  [208]=>
  string(16) "Antarctica/Troll"
  [209]=>
  string(17) "Antarctica/Vostok"
  [210]=>
  string(19) "Arctic/Longyearbyen"
  [211]=>
  string(9) "Asia/Aden"
  [212]=>
  string(11) "Asia/Almaty"
  [213]=>
  string(10) "Asia/Amman"
  [214]=>
  string(11) "Asia/Anadyr"
  [215]=>
  string(10) "Asia/Aqtau"
  [216]=>
  string(11) "Asia/Aqtobe"
  [217]=>
  string(13) "Asia/Ashgabat"
  [218]=>
  string(11) "Asia/Atyrau"
  [219]=>
  string(12) "Asia/Baghdad"
  [220]=>
  string(12) "Asia/Bahrain"
  [221]=>
  string(9) "Asia/Baku"
  [222]=>
  string(12) "Asia/Bangkok"
  [223]=>
  string(12) "Asia/Barnaul"
  [224]=>
  string(11) "Asia/Beirut"
  [225]=>
  string(12) "Asia/Bishkek"
  [226]=>
  string(11) "Asia/Brunei"
  [227]=>
  string(10) "Asia/Chita"
  [228]=>
  string(15) "Asia/Choibalsan"
  [229]=>
  string(12) "Asia/Colombo"
  [230]=>
  string(13) "Asia/Damascus"
  [231]=>
  string(10) "Asia/Dhaka"
  [232]=>
  string(9) "Asia/Dili"
  [233]=>
  string(10) "Asia/Dubai"
  [234]=>
  string(13) "Asia/Dushanbe"
  [235]=>
  string(14) "Asia/Famagusta"
  [236]=>
  string(9) "Asia/Gaza"
  [237]=>
  string(11) "Asia/Hebron"
  [238]=>
  string(16) "Asia/Ho_Chi_Minh"
  [239]=>
  string(14) "Asia/Hong_Kong"
  [240]=>
  string(9) "Asia/Hovd"
  [241]=>
  string(12) "Asia/Irkutsk"
  [242]=>
  string(12) "Asia/Jakarta"
  [243]=>
  string(13) "Asia/Jayapura"
  [244]=>
  string(14) "Asia/Jerusalem"
  [245]=>
  string(10) "Asia/Kabul"
  [246]=>
  string(14) "Asia/Kamchatka"
  [247]=>
  string(12) "Asia/Karachi"
  [248]=>
  string(14) "Asia/Kathmandu"
  [249]=>
  string(13) "Asia/Khandyga"
  [250]=>
  string(12) "Asia/Kolkata"
  [251]=>
  string(16) "Asia/Krasnoyarsk"
  [252]=>
  string(17) "Asia/Kuala_Lumpur"
  [253]=>
  string(12) "Asia/Kuching"
  [254]=>
  string(11) "Asia/Kuwait"
  [255]=>
  string(10) "Asia/Macau"
  [256]=>
  string(12) "Asia/Magadan"
  [257]=>
  string(13) "Asia/Makassar"
  [258]=>
  string(11) "Asia/Manila"
  [259]=>
  string(11) "Asia/Muscat"
  [260]=>
  string(12) "Asia/Nicosia"
  [261]=>
  string(17) "Asia/Novokuznetsk"
  [262]=>
  string(16) "Asia/Novosibirsk"
  [263]=>
  string(9) "Asia/Omsk"
  [264]=>
  string(9) "Asia/Oral"
  [265]=>
  string(15) "Asia/Phnom_Penh"
  [266]=>
  string(14) "Asia/Pontianak"
  [267]=>
  string(14) "Asia/Pyongyang"
  [268]=>
  string(10) "Asia/Qatar"
  [269]=>
  string(14) "Asia/Qyzylorda"
  [270]=>
  string(11) "Asia/Riyadh"
  [271]=>
  string(13) "Asia/Sakhalin"
  [272]=>
  string(14) "Asia/Samarkand"
  [273]=>
  string(10) "Asia/Seoul"
  [274]=>
  string(13) "Asia/Shanghai"
  [275]=>
  string(14) "Asia/Singapore"
  [276]=>
  string(18) "Asia/Srednekolymsk"
  [277]=>
  string(11) "Asia/Taipei"
  [278]=>
  string(13) "Asia/Tashkent"
  [279]=>
  string(12) "Asia/Tbilisi"
  [280]=>
  string(11) "Asia/Tehran"
  [281]=>
  string(12) "Asia/Thimphu"
  [282]=>
  string(10) "Asia/Tokyo"
  [283]=>
  string(10) "Asia/Tomsk"
  [284]=>
  string(16) "Asia/Ulaanbaatar"
  [285]=>
  string(11) "Asia/Urumqi"
  [286]=>
  string(13) "Asia/Ust-Nera"
  [287]=>
  string(14) "Asia/Vientiane"
  [288]=>
  string(16) "Asia/Vladivostok"
  [289]=>
  string(12) "Asia/Yakutsk"
  [290]=>
  string(11) "Asia/Yangon"
  [291]=>
  string(18) "Asia/Yekaterinburg"
  [292]=>
  string(12) "Asia/Yerevan"
  [293]=>
  string(15) "Atlantic/Azores"
  [294]=>
  string(16) "Atlantic/Bermuda"
  [295]=>
  string(15) "Atlantic/Canary"
  [296]=>
  string(19) "Atlantic/Cape_Verde"
  [297]=>
  string(14) "Atlantic/Faroe"
  [298]=>
  string(16) "Atlantic/Madeira"
  [299]=>
  string(18) "Atlantic/Reykjavik"
  [300]=>
  string(22) "Atlantic/South_Georgia"
  [301]=>
  string(18) "Atlantic/St_Helena"
  [302]=>
  string(16) "Atlantic/Stanley"
  [303]=>
  string(18) "Australia/Adelaide"
  [304]=>
  string(18) "Australia/Brisbane"
  [305]=>
  string(21) "Australia/Broken_Hill"
  [306]=>
  string(16) "Australia/Currie"
  [307]=>
  string(16) "Australia/Darwin"
  [308]=>
  string(15) "Australia/Eucla"
  [309]=>
  string(16) "Australia/Hobart"
  [310]=>
  string(18) "Australia/Lindeman"
  [311]=>
  string(19) "Australia/Lord_Howe"
  [312]=>
  string(19) "Australia/Melbourne"
  [313]=>
  string(15) "Australia/Perth"
  [314]=>
  string(16) "Australia/Sydney"
  [315]=>
  string(16) "Europe/Amsterdam"
  [316]=>
  string(14) "Europe/Andorra"
  [317]=>
  string(16) "Europe/Astrakhan"
  [318]=>
  string(13) "Europe/Athens"
  [319]=>
  string(15) "Europe/Belgrade"
  [320]=>
  string(13) "Europe/Berlin"
  [321]=>
  string(17) "Europe/Bratislava"
  [322]=>
  string(15) "Europe/Brussels"
  [323]=>
  string(16) "Europe/Bucharest"
  [324]=>
  string(15) "Europe/Budapest"
  [325]=>
  string(15) "Europe/Busingen"
  [326]=>
  string(15) "Europe/Chisinau"
  [327]=>
  string(17) "Europe/Copenhagen"
  [328]=>
  string(13) "Europe/Dublin"
  [329]=>
  string(16) "Europe/Gibraltar"
  [330]=>
  string(15) "Europe/Guernsey"
  [331]=>
  string(15) "Europe/Helsinki"
  [332]=>
  string(18) "Europe/Isle_of_Man"
  [333]=>
  string(15) "Europe/Istanbul"
  [334]=>
  string(13) "Europe/Jersey"
  [335]=>
  string(18) "Europe/Kaliningrad"
  [336]=>
  string(11) "Europe/Kiev"
  [337]=>
  string(12) "Europe/Kirov"
  [338]=>
  string(13) "Europe/Lisbon"
  [339]=>
  string(16) "Europe/Ljubljana"
  [340]=>
  string(13) "Europe/London"
  [341]=>
  string(17) "Europe/Luxembourg"
  [342]=>
  string(13) "Europe/Madrid"
  [343]=>
  string(12) "Europe/Malta"
  [344]=>
  string(16) "Europe/Mariehamn"
  [345]=>
  string(12) "Europe/Minsk"
  [346]=>
  string(13) "Europe/Monaco"
  [347]=>
  string(13) "Europe/Moscow"
  [348]=>
  string(11) "Europe/Oslo"
  [349]=>
  string(12) "Europe/Paris"
  [350]=>
  string(16) "Europe/Podgorica"
  [351]=>
  string(13) "Europe/Prague"
  [352]=>
  string(11) "Europe/Riga"
  [353]=>
  string(11) "Europe/Rome"
  [354]=>
  string(13) "Europe/Samara"
  [355]=>
  string(17) "Europe/San_Marino"
  [356]=>
  string(15) "Europe/Sarajevo"
  [357]=>
  string(14) "Europe/Saratov"
  [358]=>
  string(17) "Europe/Simferopol"
  [359]=>
  string(13) "Europe/Skopje"
  [360]=>
  string(12) "Europe/Sofia"
  [361]=>
  string(16) "Europe/Stockholm"
  [362]=>
  string(14) "Europe/Tallinn"
  [363]=>
  string(13) "Europe/Tirane"
  [364]=>
  string(16) "Europe/Ulyanovsk"
  [365]=>
  string(15) "Europe/Uzhgorod"
  [366]=>
  string(12) "Europe/Vaduz"
  [367]=>
  string(14) "Europe/Vatican"
  [368]=>
  string(13) "Europe/Vienna"
  [369]=>
  string(14) "Europe/Vilnius"
  [370]=>
  string(16) "Europe/Volgograd"
  [371]=>
  string(13) "Europe/Warsaw"
  [372]=>
  string(13) "Europe/Zagreb"
  [373]=>
  string(17) "Europe/Zaporozhye"
  [374]=>
  string(13) "Europe/Zurich"
  [375]=>
  string(19) "Indian/Antananarivo"
  [376]=>
  string(13) "Indian/Chagos"
  [377]=>
  string(16) "Indian/Christmas"
  [378]=>
  string(12) "Indian/Cocos"
  [379]=>
  string(13) "Indian/Comoro"
  [380]=>
  string(16) "Indian/Kerguelen"
  [381]=>
  string(11) "Indian/Mahe"
  [382]=>
  string(15) "Indian/Maldives"
  [383]=>
  string(16) "Indian/Mauritius"
  [384]=>
  string(14) "Indian/Mayotte"
  [385]=>
  string(14) "Indian/Reunion"
  [386]=>
  string(12) "Pacific/Apia"
  [387]=>
  string(16) "Pacific/Auckland"
  [388]=>
  string(20) "Pacific/Bougainville"
  [389]=>
  string(15) "Pacific/Chatham"
  [390]=>
  string(13) "Pacific/Chuuk"
  [391]=>
  string(14) "Pacific/Easter"
  [392]=>
  string(13) "Pacific/Efate"
  [393]=>
  string(17) "Pacific/Enderbury"
  [394]=>
  string(15) "Pacific/Fakaofo"
  [395]=>
  string(12) "Pacific/Fiji"
  [396]=>
  string(16) "Pacific/Funafuti"
  [397]=>
  string(17) "Pacific/Galapagos"
  [398]=>
  string(15) "Pacific/Gambier"
  [399]=>
  string(19) "Pacific/Guadalcanal"
  [400]=>
  string(12) "Pacific/Guam"
  [401]=>
  string(16) "Pacific/Honolulu"
  [402]=>
  string(18) "Pacific/Kiritimati"
  [403]=>
  string(14) "Pacific/Kosrae"
  [404]=>
  string(17) "Pacific/Kwajalein"
  [405]=>
  string(14) "Pacific/Majuro"
  [406]=>
  string(17) "Pacific/Marquesas"
  [407]=>
  string(14) "Pacific/Midway"
  [408]=>
  string(13) "Pacific/Nauru"
  [409]=>
  string(12) "Pacific/Niue"
  [410]=>
  string(15) "Pacific/Norfolk"
  [411]=>
  string(14) "Pacific/Noumea"
  [412]=>
  string(17) "Pacific/Pago_Pago"
  [413]=>
  string(13) "Pacific/Palau"
  [414]=>
  string(16) "Pacific/Pitcairn"
  [415]=>
  string(15) "Pacific/Pohnpei"
  [416]=>
  string(20) "Pacific/Port_Moresby"
  [417]=>
  string(17) "Pacific/Rarotonga"
  [418]=>
  string(14) "Pacific/Saipan"
  [419]=>
  string(14) "Pacific/Tahiti"
  [420]=>
  string(14) "Pacific/Tarawa"
  [421]=>
  string(17) "Pacific/Tongatapu"
  [422]=>
  string(12) "Pacific/Wake"
  [423]=>
  string(14) "Pacific/Wallis"
  [424]=>
  string(3) "UTC"
}
