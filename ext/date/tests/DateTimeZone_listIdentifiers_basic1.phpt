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
array(560) {
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
  string(13) "Africa/Asmera"
  [6]=>
  string(13) "Africa/Bamako"
  [7]=>
  string(13) "Africa/Bangui"
  [8]=>
  string(13) "Africa/Banjul"
  [9]=>
  string(13) "Africa/Bissau"
  [10]=>
  string(15) "Africa/Blantyre"
  [11]=>
  string(18) "Africa/Brazzaville"
  [12]=>
  string(16) "Africa/Bujumbura"
  [13]=>
  string(12) "Africa/Cairo"
  [14]=>
  string(17) "Africa/Casablanca"
  [15]=>
  string(12) "Africa/Ceuta"
  [16]=>
  string(14) "Africa/Conakry"
  [17]=>
  string(12) "Africa/Dakar"
  [18]=>
  string(20) "Africa/Dar_es_Salaam"
  [19]=>
  string(15) "Africa/Djibouti"
  [20]=>
  string(13) "Africa/Douala"
  [21]=>
  string(15) "Africa/El_Aaiun"
  [22]=>
  string(15) "Africa/Freetown"
  [23]=>
  string(15) "Africa/Gaborone"
  [24]=>
  string(13) "Africa/Harare"
  [25]=>
  string(19) "Africa/Johannesburg"
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
  string(15) "Africa/Timbuktu"
  [50]=>
  string(14) "Africa/Tripoli"
  [51]=>
  string(12) "Africa/Tunis"
  [52]=>
  string(15) "Africa/Windhoek"
  [53]=>
  string(12) "America/Adak"
  [54]=>
  string(17) "America/Anchorage"
  [55]=>
  string(16) "America/Anguilla"
  [56]=>
  string(15) "America/Antigua"
  [57]=>
  string(17) "America/Araguaina"
  [58]=>
  string(30) "America/Argentina/Buenos_Aires"
  [59]=>
  string(27) "America/Argentina/Catamarca"
  [60]=>
  string(32) "America/Argentina/ComodRivadavia"
  [61]=>
  string(25) "America/Argentina/Cordoba"
  [62]=>
  string(23) "America/Argentina/Jujuy"
  [63]=>
  string(26) "America/Argentina/La_Rioja"
  [64]=>
  string(25) "America/Argentina/Mendoza"
  [65]=>
  string(30) "America/Argentina/Rio_Gallegos"
  [66]=>
  string(23) "America/Argentina/Salta"
  [67]=>
  string(26) "America/Argentina/San_Juan"
  [68]=>
  string(26) "America/Argentina/San_Luis"
  [69]=>
  string(25) "America/Argentina/Tucuman"
  [70]=>
  string(25) "America/Argentina/Ushuaia"
  [71]=>
  string(13) "America/Aruba"
  [72]=>
  string(16) "America/Asuncion"
  [73]=>
  string(16) "America/Atikokan"
  [74]=>
  string(12) "America/Atka"
  [75]=>
  string(13) "America/Bahia"
  [76]=>
  string(16) "America/Barbados"
  [77]=>
  string(13) "America/Belem"
  [78]=>
  string(14) "America/Belize"
  [79]=>
  string(20) "America/Blanc-Sablon"
  [80]=>
  string(17) "America/Boa_Vista"
  [81]=>
  string(14) "America/Bogota"
  [82]=>
  string(13) "America/Boise"
  [83]=>
  string(20) "America/Buenos_Aires"
  [84]=>
  string(21) "America/Cambridge_Bay"
  [85]=>
  string(20) "America/Campo_Grande"
  [86]=>
  string(14) "America/Cancun"
  [87]=>
  string(15) "America/Caracas"
  [88]=>
  string(17) "America/Catamarca"
  [89]=>
  string(15) "America/Cayenne"
  [90]=>
  string(14) "America/Cayman"
  [91]=>
  string(15) "America/Chicago"
  [92]=>
  string(17) "America/Chihuahua"
  [93]=>
  string(21) "America/Coral_Harbour"
  [94]=>
  string(15) "America/Cordoba"
  [95]=>
  string(18) "America/Costa_Rica"
  [96]=>
  string(14) "America/Cuiaba"
  [97]=>
  string(15) "America/Curacao"
  [98]=>
  string(20) "America/Danmarkshavn"
  [99]=>
  string(14) "America/Dawson"
  [100]=>
  string(20) "America/Dawson_Creek"
  [101]=>
  string(14) "America/Denver"
  [102]=>
  string(15) "America/Detroit"
  [103]=>
  string(16) "America/Dominica"
  [104]=>
  string(16) "America/Edmonton"
  [105]=>
  string(16) "America/Eirunepe"
  [106]=>
  string(19) "America/El_Salvador"
  [107]=>
  string(16) "America/Ensenada"
  [108]=>
  string(18) "America/Fort_Wayne"
  [109]=>
  string(17) "America/Fortaleza"
  [110]=>
  string(17) "America/Glace_Bay"
  [111]=>
  string(15) "America/Godthab"
  [112]=>
  string(17) "America/Goose_Bay"
  [113]=>
  string(18) "America/Grand_Turk"
  [114]=>
  string(15) "America/Grenada"
  [115]=>
  string(18) "America/Guadeloupe"
  [116]=>
  string(17) "America/Guatemala"
  [117]=>
  string(17) "America/Guayaquil"
  [118]=>
  string(14) "America/Guyana"
  [119]=>
  string(15) "America/Halifax"
  [120]=>
  string(14) "America/Havana"
  [121]=>
  string(18) "America/Hermosillo"
  [122]=>
  string(28) "America/Indiana/Indianapolis"
  [123]=>
  string(20) "America/Indiana/Knox"
  [124]=>
  string(23) "America/Indiana/Marengo"
  [125]=>
  string(26) "America/Indiana/Petersburg"
  [126]=>
  string(25) "America/Indiana/Tell_City"
  [127]=>
  string(21) "America/Indiana/Vevay"
  [128]=>
  string(25) "America/Indiana/Vincennes"
  [129]=>
  string(23) "America/Indiana/Winamac"
  [130]=>
  string(20) "America/Indianapolis"
  [131]=>
  string(14) "America/Inuvik"
  [132]=>
  string(15) "America/Iqaluit"
  [133]=>
  string(15) "America/Jamaica"
  [134]=>
  string(13) "America/Jujuy"
  [135]=>
  string(14) "America/Juneau"
  [136]=>
  string(27) "America/Kentucky/Louisville"
  [137]=>
  string(27) "America/Kentucky/Monticello"
  [138]=>
  string(15) "America/Knox_IN"
  [139]=>
  string(14) "America/La_Paz"
  [140]=>
  string(12) "America/Lima"
  [141]=>
  string(19) "America/Los_Angeles"
  [142]=>
  string(18) "America/Louisville"
  [143]=>
  string(14) "America/Maceio"
  [144]=>
  string(15) "America/Managua"
  [145]=>
  string(14) "America/Manaus"
  [146]=>
  string(15) "America/Marigot"
  [147]=>
  string(18) "America/Martinique"
  [148]=>
  string(16) "America/Mazatlan"
  [149]=>
  string(15) "America/Mendoza"
  [150]=>
  string(17) "America/Menominee"
  [151]=>
  string(14) "America/Merida"
  [152]=>
  string(19) "America/Mexico_City"
  [153]=>
  string(16) "America/Miquelon"
  [154]=>
  string(15) "America/Moncton"
  [155]=>
  string(17) "America/Monterrey"
  [156]=>
  string(18) "America/Montevideo"
  [157]=>
  string(16) "America/Montreal"
  [158]=>
  string(18) "America/Montserrat"
  [159]=>
  string(14) "America/Nassau"
  [160]=>
  string(16) "America/New_York"
  [161]=>
  string(15) "America/Nipigon"
  [162]=>
  string(12) "America/Nome"
  [163]=>
  string(15) "America/Noronha"
  [164]=>
  string(27) "America/North_Dakota/Center"
  [165]=>
  string(30) "America/North_Dakota/New_Salem"
  [166]=>
  string(14) "America/Panama"
  [167]=>
  string(19) "America/Pangnirtung"
  [168]=>
  string(18) "America/Paramaribo"
  [169]=>
  string(15) "America/Phoenix"
  [170]=>
  string(22) "America/Port-au-Prince"
  [171]=>
  string(21) "America/Port_of_Spain"
  [172]=>
  string(18) "America/Porto_Acre"
  [173]=>
  string(19) "America/Porto_Velho"
  [174]=>
  string(19) "America/Puerto_Rico"
  [175]=>
  string(19) "America/Rainy_River"
  [176]=>
  string(20) "America/Rankin_Inlet"
  [177]=>
  string(14) "America/Recife"
  [178]=>
  string(14) "America/Regina"
  [179]=>
  string(16) "America/Resolute"
  [180]=>
  string(18) "America/Rio_Branco"
  [181]=>
  string(15) "America/Rosario"
  [182]=>
  string(16) "America/Santarem"
  [183]=>
  string(16) "America/Santiago"
  [184]=>
  string(21) "America/Santo_Domingo"
  [185]=>
  string(17) "America/Sao_Paulo"
  [186]=>
  string(20) "America/Scoresbysund"
  [187]=>
  string(16) "America/Shiprock"
  [188]=>
  string(21) "America/St_Barthelemy"
  [189]=>
  string(16) "America/St_Johns"
  [190]=>
  string(16) "America/St_Kitts"
  [191]=>
  string(16) "America/St_Lucia"
  [192]=>
  string(17) "America/St_Thomas"
  [193]=>
  string(18) "America/St_Vincent"
  [194]=>
  string(21) "America/Swift_Current"
  [195]=>
  string(19) "America/Tegucigalpa"
  [196]=>
  string(13) "America/Thule"
  [197]=>
  string(19) "America/Thunder_Bay"
  [198]=>
  string(15) "America/Tijuana"
  [199]=>
  string(15) "America/Toronto"
  [200]=>
  string(15) "America/Tortola"
  [201]=>
  string(17) "America/Vancouver"
  [202]=>
  string(14) "America/Virgin"
  [203]=>
  string(18) "America/Whitehorse"
  [204]=>
  string(16) "America/Winnipeg"
  [205]=>
  string(15) "America/Yakutat"
  [206]=>
  string(19) "America/Yellowknife"
  [207]=>
  string(16) "Antarctica/Casey"
  [208]=>
  string(16) "Antarctica/Davis"
  [209]=>
  string(25) "Antarctica/DumontDUrville"
  [210]=>
  string(17) "Antarctica/Mawson"
  [211]=>
  string(18) "Antarctica/McMurdo"
  [212]=>
  string(17) "Antarctica/Palmer"
  [213]=>
  string(18) "Antarctica/Rothera"
  [214]=>
  string(21) "Antarctica/South_Pole"
  [215]=>
  string(16) "Antarctica/Syowa"
  [216]=>
  string(17) "Antarctica/Vostok"
  [217]=>
  string(19) "Arctic/Longyearbyen"
  [218]=>
  string(9) "Asia/Aden"
  [219]=>
  string(11) "Asia/Almaty"
  [220]=>
  string(10) "Asia/Amman"
  [221]=>
  string(11) "Asia/Anadyr"
  [222]=>
  string(10) "Asia/Aqtau"
  [223]=>
  string(11) "Asia/Aqtobe"
  [224]=>
  string(13) "Asia/Ashgabat"
  [225]=>
  string(14) "Asia/Ashkhabad"
  [226]=>
  string(12) "Asia/Baghdad"
  [227]=>
  string(12) "Asia/Bahrain"
  [228]=>
  string(9) "Asia/Baku"
  [229]=>
  string(12) "Asia/Bangkok"
  [230]=>
  string(11) "Asia/Beirut"
  [231]=>
  string(12) "Asia/Bishkek"
  [232]=>
  string(11) "Asia/Brunei"
  [233]=>
  string(13) "Asia/Calcutta"
  [234]=>
  string(15) "Asia/Choibalsan"
  [235]=>
  string(14) "Asia/Chongqing"
  [236]=>
  string(14) "Asia/Chungking"
  [237]=>
  string(12) "Asia/Colombo"
  [238]=>
  string(10) "Asia/Dacca"
  [239]=>
  string(13) "Asia/Damascus"
  [240]=>
  string(10) "Asia/Dhaka"
  [241]=>
  string(9) "Asia/Dili"
  [242]=>
  string(10) "Asia/Dubai"
  [243]=>
  string(13) "Asia/Dushanbe"
  [244]=>
  string(9) "Asia/Gaza"
  [245]=>
  string(11) "Asia/Harbin"
  [246]=>
  string(16) "Asia/Ho_Chi_Minh"
  [247]=>
  string(14) "Asia/Hong_Kong"
  [248]=>
  string(9) "Asia/Hovd"
  [249]=>
  string(12) "Asia/Irkutsk"
  [250]=>
  string(13) "Asia/Istanbul"
  [251]=>
  string(12) "Asia/Jakarta"
  [252]=>
  string(13) "Asia/Jayapura"
  [253]=>
  string(14) "Asia/Jerusalem"
  [254]=>
  string(10) "Asia/Kabul"
  [255]=>
  string(14) "Asia/Kamchatka"
  [256]=>
  string(12) "Asia/Karachi"
  [257]=>
  string(12) "Asia/Kashgar"
  [258]=>
  string(14) "Asia/Kathmandu"
  [259]=>
  string(13) "Asia/Katmandu"
  [260]=>
  string(12) "Asia/Kolkata"
  [261]=>
  string(16) "Asia/Krasnoyarsk"
  [262]=>
  string(17) "Asia/Kuala_Lumpur"
  [263]=>
  string(12) "Asia/Kuching"
  [264]=>
  string(11) "Asia/Kuwait"
  [265]=>
  string(10) "Asia/Macao"
  [266]=>
  string(10) "Asia/Macau"
  [267]=>
  string(12) "Asia/Magadan"
  [268]=>
  string(13) "Asia/Makassar"
  [269]=>
  string(11) "Asia/Manila"
  [270]=>
  string(11) "Asia/Muscat"
  [271]=>
  string(12) "Asia/Nicosia"
  [272]=>
  string(16) "Asia/Novosibirsk"
  [273]=>
  string(9) "Asia/Omsk"
  [274]=>
  string(9) "Asia/Oral"
  [275]=>
  string(15) "Asia/Phnom_Penh"
  [276]=>
  string(14) "Asia/Pontianak"
  [277]=>
  string(14) "Asia/Pyongyang"
  [278]=>
  string(10) "Asia/Qatar"
  [279]=>
  string(14) "Asia/Qyzylorda"
  [280]=>
  string(12) "Asia/Rangoon"
  [281]=>
  string(11) "Asia/Riyadh"
  [282]=>
  string(11) "Asia/Saigon"
  [283]=>
  string(13) "Asia/Sakhalin"
  [284]=>
  string(14) "Asia/Samarkand"
  [285]=>
  string(10) "Asia/Seoul"
  [286]=>
  string(13) "Asia/Shanghai"
  [287]=>
  string(14) "Asia/Singapore"
  [288]=>
  string(11) "Asia/Taipei"
  [289]=>
  string(13) "Asia/Tashkent"
  [290]=>
  string(12) "Asia/Tbilisi"
  [291]=>
  string(11) "Asia/Tehran"
  [292]=>
  string(13) "Asia/Tel_Aviv"
  [293]=>
  string(11) "Asia/Thimbu"
  [294]=>
  string(12) "Asia/Thimphu"
  [295]=>
  string(10) "Asia/Tokyo"
  [296]=>
  string(18) "Asia/Ujung_Pandang"
  [297]=>
  string(16) "Asia/Ulaanbaatar"
  [298]=>
  string(15) "Asia/Ulan_Bator"
  [299]=>
  string(11) "Asia/Urumqi"
  [300]=>
  string(14) "Asia/Vientiane"
  [301]=>
  string(16) "Asia/Vladivostok"
  [302]=>
  string(12) "Asia/Yakutsk"
  [303]=>
  string(18) "Asia/Yekaterinburg"
  [304]=>
  string(12) "Asia/Yerevan"
  [305]=>
  string(15) "Atlantic/Azores"
  [306]=>
  string(16) "Atlantic/Bermuda"
  [307]=>
  string(15) "Atlantic/Canary"
  [308]=>
  string(19) "Atlantic/Cape_Verde"
  [309]=>
  string(15) "Atlantic/Faeroe"
  [310]=>
  string(14) "Atlantic/Faroe"
  [311]=>
  string(18) "Atlantic/Jan_Mayen"
  [312]=>
  string(16) "Atlantic/Madeira"
  [313]=>
  string(18) "Atlantic/Reykjavik"
  [314]=>
  string(22) "Atlantic/South_Georgia"
  [315]=>
  string(18) "Atlantic/St_Helena"
  [316]=>
  string(16) "Atlantic/Stanley"
  [317]=>
  string(13) "Australia/ACT"
  [318]=>
  string(18) "Australia/Adelaide"
  [319]=>
  string(18) "Australia/Brisbane"
  [320]=>
  string(21) "Australia/Broken_Hill"
  [321]=>
  string(18) "Australia/Canberra"
  [322]=>
  string(16) "Australia/Currie"
  [323]=>
  string(16) "Australia/Darwin"
  [324]=>
  string(15) "Australia/Eucla"
  [325]=>
  string(16) "Australia/Hobart"
  [326]=>
  string(13) "Australia/LHI"
  [327]=>
  string(18) "Australia/Lindeman"
  [328]=>
  string(19) "Australia/Lord_Howe"
  [329]=>
  string(19) "Australia/Melbourne"
  [330]=>
  string(15) "Australia/North"
  [331]=>
  string(13) "Australia/NSW"
  [332]=>
  string(15) "Australia/Perth"
  [333]=>
  string(20) "Australia/Queensland"
  [334]=>
  string(15) "Australia/South"
  [335]=>
  string(16) "Australia/Sydney"
  [336]=>
  string(18) "Australia/Tasmania"
  [337]=>
  string(18) "Australia/Victoria"
  [338]=>
  string(14) "Australia/West"
  [339]=>
  string(20) "Australia/Yancowinna"
  [340]=>
  string(11) "Brazil/Acre"
  [341]=>
  string(16) "Brazil/DeNoronha"
  [342]=>
  string(11) "Brazil/East"
  [343]=>
  string(11) "Brazil/West"
  [344]=>
  string(15) "Canada/Atlantic"
  [345]=>
  string(14) "Canada/Central"
  [346]=>
  string(24) "Canada/East-Saskatchewan"
  [347]=>
  string(14) "Canada/Eastern"
  [348]=>
  string(15) "Canada/Mountain"
  [349]=>
  string(19) "Canada/Newfoundland"
  [350]=>
  string(14) "Canada/Pacific"
  [351]=>
  string(19) "Canada/Saskatchewan"
  [352]=>
  string(12) "Canada/Yukon"
  [353]=>
  string(3) "CET"
  [354]=>
  string(17) "Chile/Continental"
  [355]=>
  string(18) "Chile/EasterIsland"
  [356]=>
  string(7) "CST6CDT"
  [357]=>
  string(4) "Cuba"
  [358]=>
  string(3) "EET"
  [359]=>
  string(5) "Egypt"
  [360]=>
  string(4) "Eire"
  [361]=>
  string(3) "EST"
  [362]=>
  string(7) "EST5EDT"
  [363]=>
  string(7) "Etc/GMT"
  [364]=>
  string(9) "Etc/GMT+0"
  [365]=>
  string(9) "Etc/GMT+1"
  [366]=>
  string(10) "Etc/GMT+10"
  [367]=>
  string(10) "Etc/GMT+11"
  [368]=>
  string(10) "Etc/GMT+12"
  [369]=>
  string(9) "Etc/GMT+2"
  [370]=>
  string(9) "Etc/GMT+3"
  [371]=>
  string(9) "Etc/GMT+4"
  [372]=>
  string(9) "Etc/GMT+5"
  [373]=>
  string(9) "Etc/GMT+6"
  [374]=>
  string(9) "Etc/GMT+7"
  [375]=>
  string(9) "Etc/GMT+8"
  [376]=>
  string(9) "Etc/GMT+9"
  [377]=>
  string(9) "Etc/GMT-0"
  [378]=>
  string(9) "Etc/GMT-1"
  [379]=>
  string(10) "Etc/GMT-10"
  [380]=>
  string(10) "Etc/GMT-11"
  [381]=>
  string(10) "Etc/GMT-12"
  [382]=>
  string(10) "Etc/GMT-13"
  [383]=>
  string(10) "Etc/GMT-14"
  [384]=>
  string(9) "Etc/GMT-2"
  [385]=>
  string(9) "Etc/GMT-3"
  [386]=>
  string(9) "Etc/GMT-4"
  [387]=>
  string(9) "Etc/GMT-5"
  [388]=>
  string(9) "Etc/GMT-6"
  [389]=>
  string(9) "Etc/GMT-7"
  [390]=>
  string(9) "Etc/GMT-8"
  [391]=>
  string(9) "Etc/GMT-9"
  [392]=>
  string(8) "Etc/GMT0"
  [393]=>
  string(13) "Etc/Greenwich"
  [394]=>
  string(7) "Etc/UCT"
  [395]=>
  string(13) "Etc/Universal"
  [396]=>
  string(7) "Etc/UTC"
  [397]=>
  string(8) "Etc/Zulu"
  [398]=>
  string(16) "Europe/Amsterdam"
  [399]=>
  string(14) "Europe/Andorra"
  [400]=>
  string(13) "Europe/Athens"
  [401]=>
  string(14) "Europe/Belfast"
  [402]=>
  string(15) "Europe/Belgrade"
  [403]=>
  string(13) "Europe/Berlin"
  [404]=>
  string(17) "Europe/Bratislava"
  [405]=>
  string(15) "Europe/Brussels"
  [406]=>
  string(16) "Europe/Bucharest"
  [407]=>
  string(15) "Europe/Budapest"
  [408]=>
  string(15) "Europe/Chisinau"
  [409]=>
  string(17) "Europe/Copenhagen"
  [410]=>
  string(13) "Europe/Dublin"
  [411]=>
  string(16) "Europe/Gibraltar"
  [412]=>
  string(15) "Europe/Guernsey"
  [413]=>
  string(15) "Europe/Helsinki"
  [414]=>
  string(18) "Europe/Isle_of_Man"
  [415]=>
  string(15) "Europe/Istanbul"
  [416]=>
  string(13) "Europe/Jersey"
  [417]=>
  string(18) "Europe/Kaliningrad"
  [418]=>
  string(11) "Europe/Kiev"
  [419]=>
  string(13) "Europe/Lisbon"
  [420]=>
  string(16) "Europe/Ljubljana"
  [421]=>
  string(13) "Europe/London"
  [422]=>
  string(17) "Europe/Luxembourg"
  [423]=>
  string(13) "Europe/Madrid"
  [424]=>
  string(12) "Europe/Malta"
  [425]=>
  string(16) "Europe/Mariehamn"
  [426]=>
  string(12) "Europe/Minsk"
  [427]=>
  string(13) "Europe/Monaco"
  [428]=>
  string(13) "Europe/Moscow"
  [429]=>
  string(14) "Europe/Nicosia"
  [430]=>
  string(11) "Europe/Oslo"
  [431]=>
  string(12) "Europe/Paris"
  [432]=>
  string(16) "Europe/Podgorica"
  [433]=>
  string(13) "Europe/Prague"
  [434]=>
  string(11) "Europe/Riga"
  [435]=>
  string(11) "Europe/Rome"
  [436]=>
  string(13) "Europe/Samara"
  [437]=>
  string(17) "Europe/San_Marino"
  [438]=>
  string(15) "Europe/Sarajevo"
  [439]=>
  string(17) "Europe/Simferopol"
  [440]=>
  string(13) "Europe/Skopje"
  [441]=>
  string(12) "Europe/Sofia"
  [442]=>
  string(16) "Europe/Stockholm"
  [443]=>
  string(14) "Europe/Tallinn"
  [444]=>
  string(13) "Europe/Tirane"
  [445]=>
  string(15) "Europe/Tiraspol"
  [446]=>
  string(15) "Europe/Uzhgorod"
  [447]=>
  string(12) "Europe/Vaduz"
  [448]=>
  string(14) "Europe/Vatican"
  [449]=>
  string(13) "Europe/Vienna"
  [450]=>
  string(14) "Europe/Vilnius"
  [451]=>
  string(16) "Europe/Volgograd"
  [452]=>
  string(13) "Europe/Warsaw"
  [453]=>
  string(13) "Europe/Zagreb"
  [454]=>
  string(17) "Europe/Zaporozhye"
  [455]=>
  string(13) "Europe/Zurich"
  [456]=>
  string(7) "Factory"
  [457]=>
  string(2) "GB"
  [458]=>
  string(7) "GB-Eire"
  [459]=>
  string(3) "GMT"
  [460]=>
  string(5) "GMT+0"
  [461]=>
  string(5) "GMT-0"
  [462]=>
  string(4) "GMT0"
  [463]=>
  string(9) "Greenwich"
  [464]=>
  string(8) "Hongkong"
  [465]=>
  string(3) "HST"
  [466]=>
  string(7) "Iceland"
  [467]=>
  string(19) "Indian/Antananarivo"
  [468]=>
  string(13) "Indian/Chagos"
  [469]=>
  string(16) "Indian/Christmas"
  [470]=>
  string(12) "Indian/Cocos"
  [471]=>
  string(13) "Indian/Comoro"
  [472]=>
  string(16) "Indian/Kerguelen"
  [473]=>
  string(11) "Indian/Mahe"
  [474]=>
  string(15) "Indian/Maldives"
  [475]=>
  string(16) "Indian/Mauritius"
  [476]=>
  string(14) "Indian/Mayotte"
  [477]=>
  string(14) "Indian/Reunion"
  [478]=>
  string(4) "Iran"
  [479]=>
  string(6) "Israel"
  [480]=>
  string(7) "Jamaica"
  [481]=>
  string(5) "Japan"
  [482]=>
  string(9) "Kwajalein"
  [483]=>
  string(5) "Libya"
  [484]=>
  string(3) "MET"
  [485]=>
  string(16) "Mexico/BajaNorte"
  [486]=>
  string(14) "Mexico/BajaSur"
  [487]=>
  string(14) "Mexico/General"
  [488]=>
  string(3) "MST"
  [489]=>
  string(7) "MST7MDT"
  [490]=>
  string(6) "Navajo"
  [491]=>
  string(2) "NZ"
  [492]=>
  string(7) "NZ-CHAT"
  [493]=>
  string(12) "Pacific/Apia"
  [494]=>
  string(16) "Pacific/Auckland"
  [495]=>
  string(15) "Pacific/Chatham"
  [496]=>
  string(14) "Pacific/Easter"
  [497]=>
  string(13) "Pacific/Efate"
  [498]=>
  string(17) "Pacific/Enderbury"
  [499]=>
  string(15) "Pacific/Fakaofo"
  [500]=>
  string(12) "Pacific/Fiji"
  [501]=>
  string(16) "Pacific/Funafuti"
  [502]=>
  string(17) "Pacific/Galapagos"
  [503]=>
  string(15) "Pacific/Gambier"
  [504]=>
  string(19) "Pacific/Guadalcanal"
  [505]=>
  string(12) "Pacific/Guam"
  [506]=>
  string(16) "Pacific/Honolulu"
  [507]=>
  string(16) "Pacific/Johnston"
  [508]=>
  string(18) "Pacific/Kiritimati"
  [509]=>
  string(14) "Pacific/Kosrae"
  [510]=>
  string(17) "Pacific/Kwajalein"
  [511]=>
  string(14) "Pacific/Majuro"
  [512]=>
  string(17) "Pacific/Marquesas"
  [513]=>
  string(14) "Pacific/Midway"
  [514]=>
  string(13) "Pacific/Nauru"
  [515]=>
  string(12) "Pacific/Niue"
  [516]=>
  string(15) "Pacific/Norfolk"
  [517]=>
  string(14) "Pacific/Noumea"
  [518]=>
  string(17) "Pacific/Pago_Pago"
  [519]=>
  string(13) "Pacific/Palau"
  [520]=>
  string(16) "Pacific/Pitcairn"
  [521]=>
  string(14) "Pacific/Ponape"
  [522]=>
  string(20) "Pacific/Port_Moresby"
  [523]=>
  string(17) "Pacific/Rarotonga"
  [524]=>
  string(14) "Pacific/Saipan"
  [525]=>
  string(13) "Pacific/Samoa"
  [526]=>
  string(14) "Pacific/Tahiti"
  [527]=>
  string(14) "Pacific/Tarawa"
  [528]=>
  string(17) "Pacific/Tongatapu"
  [529]=>
  string(12) "Pacific/Truk"
  [530]=>
  string(12) "Pacific/Wake"
  [531]=>
  string(14) "Pacific/Wallis"
  [532]=>
  string(11) "Pacific/Yap"
  [533]=>
  string(6) "Poland"
  [534]=>
  string(8) "Portugal"
  [535]=>
  string(3) "PRC"
  [536]=>
  string(7) "PST8PDT"
  [537]=>
  string(3) "ROC"
  [538]=>
  string(3) "ROK"
  [539]=>
  string(9) "Singapore"
  [540]=>
  string(6) "Turkey"
  [541]=>
  string(3) "UCT"
  [542]=>
  string(9) "Universal"
  [543]=>
  string(9) "US/Alaska"
  [544]=>
  string(11) "US/Aleutian"
  [545]=>
  string(10) "US/Arizona"
  [546]=>
  string(10) "US/Central"
  [547]=>
  string(15) "US/East-Indiana"
  [548]=>
  string(10) "US/Eastern"
  [549]=>
  string(9) "US/Hawaii"
  [550]=>
  string(17) "US/Indiana-Starke"
  [551]=>
  string(11) "US/Michigan"
  [552]=>
  string(11) "US/Mountain"
  [553]=>
  string(10) "US/Pacific"
  [554]=>
  string(14) "US/Pacific-New"
  [555]=>
  string(8) "US/Samoa"
  [556]=>
  string(3) "UTC"
  [557]=>
  string(4) "W-SU"
  [558]=>
  string(3) "WET"
  [559]=>
  string(4) "Zulu"
}
===DONE===