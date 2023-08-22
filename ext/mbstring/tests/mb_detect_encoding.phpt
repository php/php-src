--TEST--
mb_detect_encoding()
--EXTENSIONS--
mbstring
--INI--
mbstring.language=Japanese
--FILE--
<?php
// SJIS string (BASE64 encoded)
$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
// JIS string (BASE64 encoded)
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
// EUC-JP string
$euc_jp = "\xC6\xFC\xCB\xDC\xB8\xEC\xA5\xC6\xA5\xAD\xA5\xB9\xA5\xC8\xA4\xC7\xA4\xB9\xA1\xA301234\xA3\xB5\xA3\xB6\xA3\xB7\xA3\xB8\xA3\xB9\xA1\xA3";
// UTF-8
$polish1 = "Zażółć gęślą jaźń.";
$polish2 = "Wół poszedł spać bardzo wcześnie. A to zdanie bez ogonka.";
$hungarian = "Árvíztűrő tükörfúrógép";

echo "== BASIC TEST ==\n";

print("Empty String: " . mb_detect_encoding('') . "\n");
print("Bad ASCII (non-strict): " . mb_detect_encoding("\xDD\x92", ['ASCII', 'UTF-8'], false) . "\n");
print("Bad ASCII (strict): " . mb_detect_encoding("\xDD\x92", ['ASCII', 'UTF-8'], true) . "\n");
print("Bad ASCII/UTF-8, with more errors for ASCII (non-strict): " . mb_detect_encoding("\xD6\x8A\x8A", ['ASCII', 'UTF-8'], false) . "\n");
print("Bad ASCII/UTF-8, with more errors for ASCII (strict): " . var_export(mb_detect_encoding("\xD6\x8A\x8A", ['ASCII', 'UTF-8'], true), true) . "\n");

print("SJIS: " . mb_detect_encoding($sjis, 'SJIS', true) . "\n");
print("JIS: " . mb_detect_encoding($jis, 'JIS', true) . "\n");
print("EUC-JP (strict): " . mb_detect_encoding($euc_jp, 'UTF-8,EUC-JP,JIS', true) . "\n");
print("EUC-JP (non-strict): " . mb_detect_encoding($euc_jp, 'UTF-8,EUC-JP,JIS', false) . "\n");
print("EUC-JP (fewer choices): " . mb_detect_encoding($euc_jp, 'JIS,EUC-JP') . "\n");
print("UTF-8, polish string 1 (strict): " . mb_detect_encoding($polish1, 'UTF-8,UTF-16,ISO-8859-1', true) . "\n");
print("UTF-8, polish string 1 (non-strict): " . mb_detect_encoding($polish1, 'UTF-8,UTF-16,ISO-8859-1', false) . "\n");
print("UTF-8, polish string 2 (strict): " . mb_detect_encoding($polish2, 'UTF-8,UTF-16,ISO-8859-1', true) . "\n");
print("UTF-8, polish string 2 (non-strict): " . mb_detect_encoding($polish2, 'UTF-8,UTF-16,ISO-8859-1', false) . "\n");

echo "== ARRAY ENCODING LIST ==\n";

$a = ['UTF-8', 'EUC-JP', 'SJIS', 'JIS'];
print("JIS: " . mb_detect_encoding($jis, $a) . "\n");
print("EUC-JP: " . mb_detect_encoding($euc_jp, $a) . "\n");
print("SJIS: " . mb_detect_encoding($sjis, $a) . "\n");

$test = "CHARSET=windows-1252:Do\xeb;John";
$encodings = [
    'UTF-8', 'SJIS', 'GB2312',
    'ISO-8859-1', 'ISO-8859-2', 'ISO-8859-3', 'ISO-8859-4',
    'ISO-8859-5', 'ISO-8859-6', 'ISO-8859-7', 'ISO-8859-8', 'ISO-8859-9',
    'ISO-8859-10', 'ISO-8859-13', 'ISO-8859-14', 'ISO-8859-15', 'ISO-8859-16',
    'WINDOWS-1252', 'WINDOWS-1251', 'EUC-JP', 'EUC-TW', 'KOI8-R', 'BIG-5',
    'ISO-2022-KR', 'ISO-2022-JP', 'UTF-16'
];
echo mb_detect_encoding($test, $encodings), "\n";

$test = 'N:Müller;Jörg;;;
X-ABUID:2E4CB084-4767-4C85-BBCA-805B1DCB1C8E\:ABPerson';
echo mb_detect_encoding($test, ['UTF-8', 'SJIS']), "\n";

$test = 'BEGIN:VCARD
VERSION:2.1
N;ENCODING=QUOTED-PRINTABLE:Iksi=F1ski;Piotr
FN;ENCODING=QUOTED-PRINTABLE:Piotr Iksi=F1ski
EMAIL;PREF;INTERNET:piotr.iksinski@somedomain.com
X-GENDER:Male
REV:20080716T203548Z
END:VCARD
';
echo mb_detect_encoding($test, ['UTF-8', 'UTF-16']), "\n";

$test = 'Dušan';
echo mb_detect_encoding($test, ['UTF-8', 'ISO-8859-1']), "\n"; // Should be UTF-8

$test = 'Živko';
echo mb_detect_encoding($test, ['UTF-8', 'ISO-8859-1']), "\n"; // Should be UTF-8

// We once had a problem where all kind of strings would be detected as 'UUENCODE'
echo mb_detect_encoding('abc', ['UUENCODE', 'UTF-8']), "\n";
echo mb_detect_encoding('abc', ['UUENCODE', 'QPrint', 'HTML-ENTITIES', 'Base64', '7bit', '8bit', 'SJIS']), "\n";

// This test case courtesy of Adrien Foulon
// It depends on the below use of '+' being recognized as invalid UTF-7
$css = 'input[type="radio"]:checked + img {
    border: 5px solid #0083ca;
}';
echo mb_detect_encoding($css, mb_list_encodings(), true), "\n";

// Test cases courtesy of Kirill Roskolii and Chris Burgess
echo "-- Māori text --\n";

echo mb_detect_encoding("Total Māori,31.5,33.3,31.8,33,36.4,33.2,33.2", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
// Names of native birds from Aotearoa:
echo mb_detect_encoding("Kākā", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
echo mb_detect_encoding("Whēkau", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
echo mb_detect_encoding("Tīwaiwaka", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
echo mb_detect_encoding("Kōtuku", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
echo mb_detect_encoding("Kererū", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";
echo mb_detect_encoding("Tūī", ['UTF-8', 'ISO-8859-1', 'Windows-1251']), "\n";

echo "== DETECT ORDER ==\n";

mb_detect_order('auto');

print("JIS: " . mb_detect_encoding($jis) . "\n");

print("EUC-JP: " . mb_detect_encoding($euc_jp) . "\n");

print("SJIS: " . mb_detect_encoding($sjis) . "\n");

// Thanks to Ulrik Nielsen for the following tests; the hex strings are the same file, but in two
// different text encodings
// We do not have any strong hints showing that the second one is actually UTF-8...
// but mb_detect_encoding still guesses UTF-8 because it is the first one in the list

$win1252text = hex2bin("2320546869732066696c6520636f6e7461696e732057696e646f77732d3132353220656e636f646564206461746120616e642048544d4c20656e7469746965730a61626364650ae6f8e50af00a3c703e476f646461673c6272202f3e0a7b726561646f626a206f626a65637469643d24726573756c745b305d2e706172656e7469642061737369676e3d22646f63227d3c6272202f3e0a23205468697320697320746f20656e73757265207468617420646966666572656e74206b696e6473206f662048544d4c20656e74697469657320617265206265696e6720636f6e76657274656420636f72726563746c790af00ad00a2623383739313b0a262331373238373b0a262333383937393b0a2623353437333b0a616263646520e6f8e520f020d0203c703e476f646461673c6272202f3e207b726561646f626a206f626a65637469643d24726573756c745b305d2e706172656e7469642061737369676e3d22646f63227d3c6272202f3e202623383739313b20262331373238373b20262333383937393b202623353437333b0a232054686520666f6c6c6f77696e67206368617261637465727320617265207370656369616c206368617261637465727320746861742068617320646966666572656e7420636f646520706f696e747320696e2049534f2d383835392d3120616e642057696e646f77732d31323532202d207468617420776520646966666572656e746961746520636f72726563746c79206265747765656e2049534f2d383835392d3120616e642057696e646f77732d313235320a8c0a890a2320506f6c69736820737472696e670a50727a656a6426233337383b20646f2070727a65676c26233236313b64750a");
echo mb_detect_encoding($win1252text, ['UTF-8', 'CP1252', 'ISO-8859-1'], true), "\n";

$utf8text = hex2bin("2320546869732066696c6520636f6e7461696e73205554462d3820656e636f64656420646174610a61626364650ac3a6c3b8c3a50ac3b00a3c703e476f646461673c6272202f3e0a7b726561646f626a206f626a65637469643d24726573756c745b305d2e706172656e7469642061737369676e3d22646f63227d3c6272202f3e0a23205468697320697320746f20656e73757265207468617420646966666572656e74206b696e6473206f662048544d4c20656e74697469657320617265206265696e6720636f6e76657274656420636f72726563746c790ac3b00ac3900ae289970ae48e870ae9a1830ae195a10a616263646520c3a6c3b8c3a520c3b020c390203c703e476f646461673c6272202f3e207b726561646f626a206f626a65637469643d24726573756c745b305d2e706172656e7469642061737369676e3d22646f63227d3c6272202f3e20e2899720e48e8720e9a18320e195a10a232054686520666f6c6c6f77696e67206368617261637465727320617265207370656369616c206368617261637465727320746861742068617320646966666572656e7420636f646520706f696e747320696e2049534f2d383835392d3120616e642057696e646f77732d31323532202d207468617420776520646966666572656e746961746520636f72726563746c79206265747765656e2049534f2d383835392d3120616e642057696e646f77732d313235320ac5920ae280b00a2320506f6c69736820737472696e670a50727a656a64c5ba20646f2070727a65676cc48564750a");
echo mb_detect_encoding($utf8text, ['UTF-8', 'CP1252', 'ISO-8859-1'], true), "\n";

echo "== INVALID PARAMETER ==\n";

print("INT: " . mb_detect_encoding(1234, 'EUC-JP') . "\n"); // EUC-JP

print("EUC-JP: " . mb_detect_encoding('', 'EUC-JP') . "\n");  // SJIS

try {
    var_dump(mb_detect_encoding($euc_jp, 'BAD'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "== BOM TEST ==\n";

$str = chr(239).chr(187).chr(191).chr(195).chr(180); // UTF-8 BOM followed by ô
var_dump(mb_detect_encoding($str, ['UTF-8', 'ISO-8859-1'], true));
// U+4E4E is the Chinese character 乎; normally it would be impossible to distinguish UTF-16LE from UTF-16BE
// But the BOM can tell us which one it is
var_dump(mb_detect_encoding("\xFE\xFF\x4E\x4E", ['UTF-8', 'ISO-8859-1', 'UTF-16LE', 'UTF-16BE'], true));
var_dump(mb_detect_encoding("\xFF\xFE\x4E\x4E", ['UTF-8', 'ISO-8859-1', 'UTF-16LE', 'UTF-16BE'], true));
// However, a BOM should only appear at the beginning of the string
$detected = mb_detect_encoding("\x4E\x4E\xFE\xFF\x4E\x4E", ['UTF-8', 'ISO-8859-1', 'UTF-16LE', 'UTF-16BE'], true);
if ($detected === 'UTF-16BE' || $detected === 'UTF-16LE')
    die("Don't accept a BOM in the middle of a string");

echo "== CHECK FUNCTION TEST ==\n";

function testCheckFn($str, $encoding, $encodings) {
    if (mb_check_encoding($str, $encoding))
        die("Input string " . bin2hex($str) . " should not be valid in " . $encoding);
    if (mb_detect_encoding($str, $encodings, true) === $encoding)
        die("mb_detect_encoding should never return " . $encoding . " for invalid input string " . bin2hex($str));
}

testCheckFn("abc + abc", "UTF-7", "UTF-7,UTF-8");

echo "== TORTURE TEST ==\n";

function test($strings, $encodings) {
    foreach ($strings as $example) {
        foreach ($encodings as $encoding) {
            $converted = mb_convert_encoding($example, $encoding, 'UTF-8');
            $detected = mb_detect_encoding($converted, $encodings);
            if ($detected !== $encoding) {
                echo "BAD! mb_detect_encoding returned $detected (should have been $encoding)\n";
                echo "UTF-8 was: $example\n";
                echo "$encoding bytes: ", bin2hex($converted), "\n";
            }
        }
    }
}

$jpStrings = [
    // Hat tip to Wikipedia
    "日本で生まれ育ったほとんどの人は、日本語を母語とする[注 3]",
    "2019年4月現在、インターネット上の言語使用者数は、英語、中国語、スペイン語、アラビア語、ポルトガル語、マレー語/インドネシア語、フランス語に次いで8番目に多い[13][信頼性要検証]。",
    "日本語は地方ごとに多様な方言があり、とりわけ琉球諸島[要曖昧さ回避]で方言差が著しい（「方言」の節参照）。",
    "さ  し   す   せ   そ   しゃ  しゅ  しょ  （清音）
     た   ち   つ   て   と   ちゃ  ちゅ  ちょ  （清音）
     な   に   ぬ   ね   の   にゃ  にゅ  にょ  ――",
    "明治時代に入り、1889年から大槻文彦編の小型辞書『言海』が刊行された。これは、古典語・日常語を網羅し、五十音順に見出しを並べて、品詞・漢字表記・語釈を付した初の近代的な日本語辞書であった。『言海』は、後の辞書の模範的存在となり、後に増補版の『大言海』も刊行された。",
    "奈良時代には『楊氏漢語抄』や『弁色立成（べんしきりゅうじょう）』という辞書が編纂された。それぞれ逸文として残るのみであるが、和訓を有する漢和辞書であったらしい。",
    "複雑な文字体系を理由に、日本語を特殊とする議論もある。",
    "一時的流行語。ある時代の若い世代が使う言葉。戦後の「アジャパー」、1970年代の「チカレタビー」など。コーホート語（同世代語）。",
    "外国人による日本語研究も、中世末期から近世前期にかけて多く行われた。イエズス会では日本語とポルトガル語の辞書『日葡辞書』（1603年）が編纂され、また、同会のロドリゲスにより文法書『日本大文典』（1608年）および『日本小文典』（1620年）が表された。",
    "一方、戦後になると各地の方言が失われつつあることが危惧されるようになった。NHK放送文化研究所は、（昭和20年代の時点で）各地の純粋な方言は80歳以上の老人の間でのみ使われているにすぎないとして、1953年から5年計画で全国の方言の録音を行った。",
    "文体史 和漢混淆文の誕生",
    "仮名遣いについては、早く小学校令施行規則（1900年）において、「にんぎやう（人形）」を「にんぎょー」とするなど、漢字音を発音通りにする、いわゆる「棒引き仮名遣い」が採用されたことがあった。",
    "元来、日本に文字と呼べるものはなく、言葉を表記するためには中国渡来の漢字を用いた（いわゆる神代文字は後世の偽作とされている[167]）。",
    "第二次世界大戦が激しくなるにつれて、外来語を禁止または自粛する風潮も起こったが、戦後はアメリカ発の外来語が爆発的に多くなった。",
    "そこから類推した結果、「文字を読む」に対して「文字が読むる（読める）」などの可能動詞が出来上がったものと考えられる。",
    "近代以降には、外国語（特に英語）の音の影響で新しい音が使われ始めた。比較的一般化した「シェ・チェ・ツァ・ツェ・ツォ・ティ・ファ・フィ・フェ・フォ・ジェ・ディ・デュ」などの音に加え、場合によっては、「イェ・ウィ・ウェ・ウォ・クァ・クィ・クェ・クォ・ツィ・トゥ・グァ・ドゥ・テュ・フュ」などの音も使われる[147]。",
    "20世紀後半から21世紀初頭にかけて中央競馬のトップジョッキーとして活躍し、競馬ファンから名手の愛称で親しまれた。",
    "名鉄モ600形電車（めいてつモ600がたでんしゃ）は、名古屋鉄道（名鉄）が岐阜地区の直流600 V電化路線区の一つである美濃町線において運用する目的で、1970年（昭和45年）に導入した電車である。",
    "その視点から、真理は当初未就学期の娘を幼稚園に入園させる考えは持っていなかったが、",
    // And here's to everyone's favorite blue robot...
    "機械だって 涙を流して 震えながら 勇気を叫ぶだろう",
    "台風だって 心を痛めて 愛を込めて さよならするだろう",
    "便利な道具で　助けてくれる　おもちゃの　兵器だ　「ソレ！とつげき！」"
];
$jpEncodings = [
    'UTF-32BE',
    'UTF-32LE',
    'UTF-16BE',
    'UTF-16LE',
    'UTF-8',
    'UTF-7',
    'EUC-JP',
    'SJIS',
    'ISO-2022-JP'
];
test($jpStrings, $jpEncodings);

$cnStrings = [
    // Headline randomly picked from Chinese news
    "日本宫内厅宣布，真子公主和小室圭将在10月26日完婚。",
    // The Dream of Red Mansions
    "此开卷第一回也。作者自云曾历过一番梦幻之后，故将真事隐去，而借“通灵”说此《石头记》一书也",
    "一日，炎夏永昼，士隐于书房闲坐，手倦抛书，伏几盹睡。",
    "　　须臾，茶毕，早已设下杯盘。",
    "士隐听了，便迎上来道：“你满口说些什么？只听见些“好了”“好了”。",
    "士隐送雨村去后，回房一觉，直至红日三竿方醒。",
    "时逢三五便团圞，满把清光护玉栏。",
    "但弟子愚拙，不能洞悉明白。",
    "按那石头上书云：当日地陷东南，这东南有个姑苏城，城中阊门",
    "后来既受天地精华，复得甘露滋养，遂脱了草木之胎，幻化人形，仅仅修成女体，终日游于“离恨天”外，饥餐“秘情果”，渴饮“灌愁水”。",
    "原来雨村自那日见了甄家丫鬟，曾回顾他两次，自谓是个知己，便时刻放在心上。",
    // Wikipedia
    // (A lot of this uses traditional Chinese characters, which we also want to be tested)
    "漢语主要使用漢字書寫，為語素文字。",
    "現漢字擁有兩套文字系統，分別為正體字與簡體字。",
    "標準漢語中四個主要的聲調，使用ma這個音節發音。",
    "在語言學原則上，互相之間不能通話的應該被定性為語言而非方言。",
    "但不少詞彙會採用粵語詞彙（例如採用「巴士」而非「公車」，採用「魚蛋」而非「魚丸子」，採用「沙律」而非「色拉」）",
    "這是因為其他國家（除日本外）均使用表音文字，對於“文”[6]與“語”[7]並不作區分，不符合漢語語法",
    "主条目：闽语、閩東語、福州語、閩南語和臺灣閩南語",
    "普通話中，ai，ei，ao，ou等都是雙元音韻母",
    "汉字",
    "实词，词汇中含有实际意义的词语",
    "我的老師　一位顧客　恭敬地鞠躬　完全相信　非常堅強　多麼可愛",
    "敬畏生命　熱愛工作　上中學　登泰山　蓋房子　包餃子",
    "参见：外來語 § 漢語外來語、中文外來語、汉字文化圈和汉字复活",
    "如果将汉语延深入汉文，则汉文的信息密度更大。",
    "我們家蓋了新房子。",
    "他是一個高而瘦的老人。",
    "我們家的臺階低。",
    "我們家蓋了新房子。",
    "敵人監視着葦塘。",
    "連詞：用來連接詞、短語或句子，表示前後有並列、遞進、轉折、因果、假設等關係。",
    "「大去之期不遠矣」",
    "“官话方言”绝大多数次级方言都没有入声",
    "其中，闽南语不仅有 −p，−t，−k，也有模糊入声"
];
$cnEncodings = [
    'UTF-32BE',
    'UTF-32LE',
    'UTF-16BE',
    'UTF-16LE',
    'UTF-8',
    'UTF-7',
    'GB18030',
    'BIG-5'
];
test($cnStrings, $cnEncodings);

$deStrings = [
    // Much love to Wikipedia
    "Die beiden Brücken über den Strelasund (2011)",
    "die Rügenbrücke und der Rügendamm sowie die regelmäßig betriebenen Fährverbindungen zwischen Stralsund",
    "Der „Rügendamm“ ist die erste feste Strelasundquerung",
    "Koordinaten    ♁54° 18′ 39″ N, 13° 7′ 0″ O",
    "Die ausschließlich dem Kraftfahrzeugverkehr",
    "Die Brücke ermöglicht dem Schiffsverkehr eine Durchfahrtshöhe von 40 m.[1]",
    "Nach der Hauptbrücke folgt die Vorlandbrücke Dänholm (BW 3), eine 532,3 m",
    "Die alte, als Klappbrücke ausgeführte Ziegelgrabenbrücke ist 133 Meter lang",
    "Vor allem das Fährdorf Stralow („stral“ bedeutet im Mittelniederdeutschen und im Slawischen „Pfeil“) entwickelte sich rasch.",
    "1946 kam es aufgrund der Zerstörung der Brücken",
    "Auf der Trajektstrecke verkehrten im ersten Jahr bereits 90.000 Fahrgäste",
    "Mai 1897 zwei Schnellzugpaare zwischen Berlin und Saßnitz.",
    "Der Damm im Ziegelgraben und zwischen dem Dänholm und dem Widerlager der Brücke wurde mit den bei den Eisenbahnarbeiten gewonnenen Böden verfüllt.",
    "Dabei passierten die vier anderen Trajekte die „Altefähr“",
    "Ebenfalls in den 1980er Jahren traten zunehmend Ermüdungserscheinungen an den stark beanspruchten Stahlüberbauten auf",
    "Erste Planungen für einen neuen Rügendamm
Die Kapazität der Eisenbahnbrücke war begrenzt:",
    "bestehend aus den Firmen Walter Bau AG vereinigt mit Dywidag (später/nach v.g. Insolvenz durch die Dywidag Bau GmbH)",
    "Bereits im Herbst 1998 erfolgten die ersten Bohrungen zur Untersuchung der Tragfähigkeit des Baugrundes im Bereich des Ziegelgrabens"
];
$deEncodings = [
    'UTF-32BE',
    'UTF-32LE',
    'UTF-16BE',
    'UTF-16LE',
    'UTF-8',
    'ISO-8859-1'
    // TODO: It would be good if ISO-8859-2 and ISO-8859-15 can be accurately detected as well
];
test($deStrings, $deEncodings);

test([$polish1, $polish2], ['UTF-32BE', 'UTF-32LE', 'UTF-16BE', 'UTF-16LE', 'UTF-8', 'ISO-8859-2']);

$czechStrings = [
    // Gotta love these Czech proverbs
    // "Some like girls, others like muffins." Truer words were never spoken.
    'Bezdomovec je doma všude.',
    'Bez práce nejsou koláče.',
    'Bez peněz do hospody nelez.',
    'Bližší košile nežli kabát.',
    'Boží mlýny melou pomalu, ale jistě.',
    'Co je dovoleno pánovi, není dovoleno kmánovi.',
    'Co je šeptem, to je s čertem.',
    'Co je v domě, není pro mě.',
    'Co je v domě, to se počítá.',
    'Co jsi z úst vypustil, ani párem koní nedostaneš zpět.',
    'Co můžeš udělat dnes, neodkládej na zítřek.',
    'Co nejde po dobrým, to půjde po zlým.',
    'Co oči nevidí, to srdce nebolí (a ruce neukradnou).',
    'Co se škádlívá, to se rádo mívá.',
    'Co se v mládí naučíš, ke stáru jako když najdeš.',
    'Co sis uvařil, to si sněz.',
    'Co tě nezabije, to tě posílí.',
    'Cvik dělá mistra.',
    'Co tě nepálí, nehas.',
    'Co na srdci, to na jazyku.',
    'Co nejde silou, jde rozumem.',
    'Čas všechny rány zahojí.',
    'Častá krůpěj kámen proráží.',
    'Čím výše vystoupíš, tím hlouběji padáš.',
    'Čím výše vystoupíš, tím větší rozhled.',
    'Čiň čertu dobře, peklem se ti odmění.',
    'Čistota – půl zdraví.',
    'Dal ses na vojnu, tak bojuj.',
    'Darovanému koni na zuby nehleď.',
    'Devatero řemesel – desátá bída.',
    'Dějiny píší vítězové.',
    'Dobré slovo i železná vrata otvírá.',
    'Dočkej času jako husa klasu.',
    'Drzé čelo lepší než poplužní dvůr.',
    'Dvakrát měř, jednou řež.',
    'Důvěřuj, ale prověřuj',
    'Hlad je nejlepší kuchař.',
    'Hlad má velké oči.',
    'Hloupý, kdo dává, hloupější, kdo nebere.',
    'Hněv je špatný rádce.',
    'Někdo rád holky, jinej zas vdolky.',
    'Volům kroky a jelenům skoky.',
    'Vrána k vráně sedá, rovný rovného si hledá.',
    'Všeho nechám, už tam spěchám.',
    'Všechna sláva, polní tráva.',
    'Všeho s mírou.',
    'Všechno zlé je pro něco dobré.',
    'Všude dobře, doma nejlépe.',
    'Vrána vráně oči nevyklove.',
    'Výjimka potvrzuje pravidlo.',
    'Vzduch – boží duch.',
    'Za dobrotu na žebrotu.',
    'Zadarmo ani kuře nehrabe.',
    'Zahálky jsa služebníkem, neběduj, žes hadrníkem.',
    'Z cizího krev neteče.',
    'Zítra je taky den.',
    'Zakázané ovoce chutná nejlépe.',
    'Zlaté slovo, které zůstane v ústech.',
    'Zvyk je železná košile.',
    'Žába močál vždy najde.',
    'Žádná píseň není tak dlouhá, aby jí nebyl konec.',
    'Žádný strom neroste do nebe.',
    'Žádný učený z nebe nespadl.',
    'Žízeň je věčná.'
];
$czechEncodings = [
    'UTF-8',
    'UTF-16',
    // 'Windows-1250', // Windows-1250 is not supported by mbstring
    // 'ISO-8859-2' // We are not able to accurately distinguish UTF-8 and ISO-8859-2
];
test($czechStrings, $czechEncodings);

test([$hungarian], ['UTF-8', 'UTF-16', 'Windows-1252']);

$turkishStrings = [
    // Random junk indiscriminately copied from randomly picked Wikipedia articles
    "Samsun 19 Mayıs Stadyumu, Samsun'un Tekkeköy ilçesinde bulunan akıllı çok amaçlı stadyumdur. 33.919 koltuk kapasitesiyle Samsunspor'un iç saha maçlarına ev sahipliği yapmaktadır. Toplu Konut İdaresi tarafından yaptırılan ve 2017'de tamamlanan stadyum adını Mustafa Kemal'in Samsun'a çıktığı gün olan 19 Mayıs'tan almaktadır. İlk olarak Nisan 2011'de duyurulan proje 3 Aralık 2012'de Toplu Konut İdaresince yapılan ve Ali Acar İnşaat'ın kazandığı ihale ile resmiyete dökülmüş, 4 Ağustos 2013 tarihindeki temel atma töreni ile de inşa aşamasına geçilmiştir. İnşaatın başlangıç tarihinden itibaren en geç 800 gün içerisinde tamamlanması taahhüt edilse de UEFA'nın standartlarına uyum sağlamak için projede yenileme yapılması, çatının inşasıyla sorumlu şirketin iflas etmesi gibi sebeplerle tamamlanma süresi birkaç kez sarkmıştır. Bu bağlamda, ilk etapta 2014-15 sezonunda hazır hâle geleceği açıklanan stadyumun açılışı önce 2015-16, daha sonra 2016-17 sezonu başına ertelense de bu hedefler de yakalanamamıştır.",
    "Lütf-i Celil (Osmanlı Türkçesi: ﻟﻄﻒ ﺟﻠﻴﻞ Anlamı: \"İlahi Lütuf\"), Osmanlı Donanması'nın Lütf-i Celil sınıfının öncü gemisi olan zırhlı savaş gemisidir. Başlangıçta Osmanlı İmparatorluğu'na bağlı özerk bir devlet olan Mısır Hidivliği tarafından sipariş edilen Lütf-i Celil, Osmanlı hükûmetinin Mısır'ı gemiyi teslim etmeye zorlaması ile Fransa'daki Forges et Chantiers de la Gironde tersanesinde yapım aşamasındayken Osmanlılara devredildi. Lütf-i Celil, 1877'de 93 Harbi sırasında aktif görevde bulundu ve Rus güçlerinin Tuna'yı geçmesini önlemek için operasyonlarda bulundu. 11 Mayıs'ta devriye gezerken bir Rus topçu bataryasıyla çatışmaya girdi.",
    "Çoğu tarihçinin kanısına göre, ABD'nin üçüncü başkanı Jefferson, karısının ölümünün ardından kölesi Sally Hemings ile 38 yıl süren ilişkisi sırasında onun altı çocuğunun babası olmuştur.",
    "2011 İran drama filmi Bir Ayrılık, 61. Berlin Film Festivali'nde Altın Ayı kazanarak, bu ödülü alan ilk İran filmi oldu.",
    "Josef Bringas, isyan eden general Nikiforos Fokas'a karşı Konstantinopolis'e birlikler getirdi; asilerin Boğaziçi'ni geçmelerini engellemek için tüm gemileri tuttu; Nikiforos'un babası Bardas'ı rehin aldı ama başarılı olamadı."
];
// ISO-8859-9 and Windows-1254 are very similar and we can't really distinguish them from each other
// But both of them should be distinguishable from UTF-8
test($turkishStrings, ['UTF-8', 'UTF-16', 'ISO-8859-9']);
test($turkishStrings, ['UTF-8', 'Windows-1254']);

echo "Done!\n";

?>
--EXPECT--
== BASIC TEST ==
Empty String: ASCII
Bad ASCII (non-strict): UTF-8
Bad ASCII (strict): UTF-8
Bad ASCII/UTF-8, with more errors for ASCII (non-strict): UTF-8
Bad ASCII/UTF-8, with more errors for ASCII (strict): false
SJIS: SJIS
JIS: JIS
EUC-JP (strict): EUC-JP
EUC-JP (non-strict): EUC-JP
EUC-JP (fewer choices): EUC-JP
UTF-8, polish string 1 (strict): UTF-8
UTF-8, polish string 1 (non-strict): UTF-8
UTF-8, polish string 2 (strict): UTF-8
UTF-8, polish string 2 (non-strict): UTF-8
== ARRAY ENCODING LIST ==
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
ISO-8859-1
UTF-8
UTF-8
UTF-8
UTF-8
UTF-8
SJIS
UTF-8
-- Māori text --
UTF-8
UTF-8
UTF-8
UTF-8
UTF-8
UTF-8
UTF-8
== DETECT ORDER ==
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
Windows-1252
UTF-8
== INVALID PARAMETER ==
INT: EUC-JP
EUC-JP: EUC-JP
mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "BAD"
== BOM TEST ==
string(5) "UTF-8"
string(8) "UTF-16BE"
string(8) "UTF-16LE"
== CHECK FUNCTION TEST ==
== TORTURE TEST ==
Done!
