--TEST--
Bug #37706 (Test LOB locator reuse. Extends simple test of lob_037.phpt)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

define('NUMLOBS', 200);

require(dirname(__FILE__).'/connect.inc');
require(dirname(__FILE__).'/create_table.inc');

for ($i = 0; $i < NUMLOBS; $i++) {
	$s = oci_parse($c, "insert into ".$schema.$table_name." (id, clob) values(".$i.", '".$i."aaaa".$i.$i."aaaaaaaaaaaaaaaaaaaaaaaaaaaz')");
	oci_execute($s);
}

echo "Test 1: CLOB as locator\n";

$s = oci_parse($c, "select clob from ".$schema.$table_name." order by id");
oci_execute($s);

$row = array();
for ($i = 0; $i < NUMLOBS; $i++) {
	$row[$i] = oci_fetch_array($s, OCI_NUM);
}

for ($i = 0; $i < NUMLOBS; $i++) {
	echo "Row $i Size:  " . $row[$i][0]->size() . "\n";
	echo "Pos 1: " . $row[$i][0]->tell() . "\n";
	echo "Data:  " . $row[$i][0]->read(5) . "\n";
	echo "Pos 2: " . $row[$i][0]->tell() . "\n";
	echo "Data:  " . $row[$i][0]->read(12) . "\n";
}

echo "Done\n";

?>
--EXPECT--
Test 1: CLOB as locator
Row 0 Size:  35
Pos 1: 0
Data:  0aaaa
Pos 2: 5
Data:  00aaaaaaaaaa
Row 1 Size:  35
Pos 1: 0
Data:  1aaaa
Pos 2: 5
Data:  11aaaaaaaaaa
Row 2 Size:  35
Pos 1: 0
Data:  2aaaa
Pos 2: 5
Data:  22aaaaaaaaaa
Row 3 Size:  35
Pos 1: 0
Data:  3aaaa
Pos 2: 5
Data:  33aaaaaaaaaa
Row 4 Size:  35
Pos 1: 0
Data:  4aaaa
Pos 2: 5
Data:  44aaaaaaaaaa
Row 5 Size:  35
Pos 1: 0
Data:  5aaaa
Pos 2: 5
Data:  55aaaaaaaaaa
Row 6 Size:  35
Pos 1: 0
Data:  6aaaa
Pos 2: 5
Data:  66aaaaaaaaaa
Row 7 Size:  35
Pos 1: 0
Data:  7aaaa
Pos 2: 5
Data:  77aaaaaaaaaa
Row 8 Size:  35
Pos 1: 0
Data:  8aaaa
Pos 2: 5
Data:  88aaaaaaaaaa
Row 9 Size:  35
Pos 1: 0
Data:  9aaaa
Pos 2: 5
Data:  99aaaaaaaaaa
Row 10 Size:  38
Pos 1: 0
Data:  10aaa
Pos 2: 5
Data:  a1010aaaaaaa
Row 11 Size:  38
Pos 1: 0
Data:  11aaa
Pos 2: 5
Data:  a1111aaaaaaa
Row 12 Size:  38
Pos 1: 0
Data:  12aaa
Pos 2: 5
Data:  a1212aaaaaaa
Row 13 Size:  38
Pos 1: 0
Data:  13aaa
Pos 2: 5
Data:  a1313aaaaaaa
Row 14 Size:  38
Pos 1: 0
Data:  14aaa
Pos 2: 5
Data:  a1414aaaaaaa
Row 15 Size:  38
Pos 1: 0
Data:  15aaa
Pos 2: 5
Data:  a1515aaaaaaa
Row 16 Size:  38
Pos 1: 0
Data:  16aaa
Pos 2: 5
Data:  a1616aaaaaaa
Row 17 Size:  38
Pos 1: 0
Data:  17aaa
Pos 2: 5
Data:  a1717aaaaaaa
Row 18 Size:  38
Pos 1: 0
Data:  18aaa
Pos 2: 5
Data:  a1818aaaaaaa
Row 19 Size:  38
Pos 1: 0
Data:  19aaa
Pos 2: 5
Data:  a1919aaaaaaa
Row 20 Size:  38
Pos 1: 0
Data:  20aaa
Pos 2: 5
Data:  a2020aaaaaaa
Row 21 Size:  38
Pos 1: 0
Data:  21aaa
Pos 2: 5
Data:  a2121aaaaaaa
Row 22 Size:  38
Pos 1: 0
Data:  22aaa
Pos 2: 5
Data:  a2222aaaaaaa
Row 23 Size:  38
Pos 1: 0
Data:  23aaa
Pos 2: 5
Data:  a2323aaaaaaa
Row 24 Size:  38
Pos 1: 0
Data:  24aaa
Pos 2: 5
Data:  a2424aaaaaaa
Row 25 Size:  38
Pos 1: 0
Data:  25aaa
Pos 2: 5
Data:  a2525aaaaaaa
Row 26 Size:  38
Pos 1: 0
Data:  26aaa
Pos 2: 5
Data:  a2626aaaaaaa
Row 27 Size:  38
Pos 1: 0
Data:  27aaa
Pos 2: 5
Data:  a2727aaaaaaa
Row 28 Size:  38
Pos 1: 0
Data:  28aaa
Pos 2: 5
Data:  a2828aaaaaaa
Row 29 Size:  38
Pos 1: 0
Data:  29aaa
Pos 2: 5
Data:  a2929aaaaaaa
Row 30 Size:  38
Pos 1: 0
Data:  30aaa
Pos 2: 5
Data:  a3030aaaaaaa
Row 31 Size:  38
Pos 1: 0
Data:  31aaa
Pos 2: 5
Data:  a3131aaaaaaa
Row 32 Size:  38
Pos 1: 0
Data:  32aaa
Pos 2: 5
Data:  a3232aaaaaaa
Row 33 Size:  38
Pos 1: 0
Data:  33aaa
Pos 2: 5
Data:  a3333aaaaaaa
Row 34 Size:  38
Pos 1: 0
Data:  34aaa
Pos 2: 5
Data:  a3434aaaaaaa
Row 35 Size:  38
Pos 1: 0
Data:  35aaa
Pos 2: 5
Data:  a3535aaaaaaa
Row 36 Size:  38
Pos 1: 0
Data:  36aaa
Pos 2: 5
Data:  a3636aaaaaaa
Row 37 Size:  38
Pos 1: 0
Data:  37aaa
Pos 2: 5
Data:  a3737aaaaaaa
Row 38 Size:  38
Pos 1: 0
Data:  38aaa
Pos 2: 5
Data:  a3838aaaaaaa
Row 39 Size:  38
Pos 1: 0
Data:  39aaa
Pos 2: 5
Data:  a3939aaaaaaa
Row 40 Size:  38
Pos 1: 0
Data:  40aaa
Pos 2: 5
Data:  a4040aaaaaaa
Row 41 Size:  38
Pos 1: 0
Data:  41aaa
Pos 2: 5
Data:  a4141aaaaaaa
Row 42 Size:  38
Pos 1: 0
Data:  42aaa
Pos 2: 5
Data:  a4242aaaaaaa
Row 43 Size:  38
Pos 1: 0
Data:  43aaa
Pos 2: 5
Data:  a4343aaaaaaa
Row 44 Size:  38
Pos 1: 0
Data:  44aaa
Pos 2: 5
Data:  a4444aaaaaaa
Row 45 Size:  38
Pos 1: 0
Data:  45aaa
Pos 2: 5
Data:  a4545aaaaaaa
Row 46 Size:  38
Pos 1: 0
Data:  46aaa
Pos 2: 5
Data:  a4646aaaaaaa
Row 47 Size:  38
Pos 1: 0
Data:  47aaa
Pos 2: 5
Data:  a4747aaaaaaa
Row 48 Size:  38
Pos 1: 0
Data:  48aaa
Pos 2: 5
Data:  a4848aaaaaaa
Row 49 Size:  38
Pos 1: 0
Data:  49aaa
Pos 2: 5
Data:  a4949aaaaaaa
Row 50 Size:  38
Pos 1: 0
Data:  50aaa
Pos 2: 5
Data:  a5050aaaaaaa
Row 51 Size:  38
Pos 1: 0
Data:  51aaa
Pos 2: 5
Data:  a5151aaaaaaa
Row 52 Size:  38
Pos 1: 0
Data:  52aaa
Pos 2: 5
Data:  a5252aaaaaaa
Row 53 Size:  38
Pos 1: 0
Data:  53aaa
Pos 2: 5
Data:  a5353aaaaaaa
Row 54 Size:  38
Pos 1: 0
Data:  54aaa
Pos 2: 5
Data:  a5454aaaaaaa
Row 55 Size:  38
Pos 1: 0
Data:  55aaa
Pos 2: 5
Data:  a5555aaaaaaa
Row 56 Size:  38
Pos 1: 0
Data:  56aaa
Pos 2: 5
Data:  a5656aaaaaaa
Row 57 Size:  38
Pos 1: 0
Data:  57aaa
Pos 2: 5
Data:  a5757aaaaaaa
Row 58 Size:  38
Pos 1: 0
Data:  58aaa
Pos 2: 5
Data:  a5858aaaaaaa
Row 59 Size:  38
Pos 1: 0
Data:  59aaa
Pos 2: 5
Data:  a5959aaaaaaa
Row 60 Size:  38
Pos 1: 0
Data:  60aaa
Pos 2: 5
Data:  a6060aaaaaaa
Row 61 Size:  38
Pos 1: 0
Data:  61aaa
Pos 2: 5
Data:  a6161aaaaaaa
Row 62 Size:  38
Pos 1: 0
Data:  62aaa
Pos 2: 5
Data:  a6262aaaaaaa
Row 63 Size:  38
Pos 1: 0
Data:  63aaa
Pos 2: 5
Data:  a6363aaaaaaa
Row 64 Size:  38
Pos 1: 0
Data:  64aaa
Pos 2: 5
Data:  a6464aaaaaaa
Row 65 Size:  38
Pos 1: 0
Data:  65aaa
Pos 2: 5
Data:  a6565aaaaaaa
Row 66 Size:  38
Pos 1: 0
Data:  66aaa
Pos 2: 5
Data:  a6666aaaaaaa
Row 67 Size:  38
Pos 1: 0
Data:  67aaa
Pos 2: 5
Data:  a6767aaaaaaa
Row 68 Size:  38
Pos 1: 0
Data:  68aaa
Pos 2: 5
Data:  a6868aaaaaaa
Row 69 Size:  38
Pos 1: 0
Data:  69aaa
Pos 2: 5
Data:  a6969aaaaaaa
Row 70 Size:  38
Pos 1: 0
Data:  70aaa
Pos 2: 5
Data:  a7070aaaaaaa
Row 71 Size:  38
Pos 1: 0
Data:  71aaa
Pos 2: 5
Data:  a7171aaaaaaa
Row 72 Size:  38
Pos 1: 0
Data:  72aaa
Pos 2: 5
Data:  a7272aaaaaaa
Row 73 Size:  38
Pos 1: 0
Data:  73aaa
Pos 2: 5
Data:  a7373aaaaaaa
Row 74 Size:  38
Pos 1: 0
Data:  74aaa
Pos 2: 5
Data:  a7474aaaaaaa
Row 75 Size:  38
Pos 1: 0
Data:  75aaa
Pos 2: 5
Data:  a7575aaaaaaa
Row 76 Size:  38
Pos 1: 0
Data:  76aaa
Pos 2: 5
Data:  a7676aaaaaaa
Row 77 Size:  38
Pos 1: 0
Data:  77aaa
Pos 2: 5
Data:  a7777aaaaaaa
Row 78 Size:  38
Pos 1: 0
Data:  78aaa
Pos 2: 5
Data:  a7878aaaaaaa
Row 79 Size:  38
Pos 1: 0
Data:  79aaa
Pos 2: 5
Data:  a7979aaaaaaa
Row 80 Size:  38
Pos 1: 0
Data:  80aaa
Pos 2: 5
Data:  a8080aaaaaaa
Row 81 Size:  38
Pos 1: 0
Data:  81aaa
Pos 2: 5
Data:  a8181aaaaaaa
Row 82 Size:  38
Pos 1: 0
Data:  82aaa
Pos 2: 5
Data:  a8282aaaaaaa
Row 83 Size:  38
Pos 1: 0
Data:  83aaa
Pos 2: 5
Data:  a8383aaaaaaa
Row 84 Size:  38
Pos 1: 0
Data:  84aaa
Pos 2: 5
Data:  a8484aaaaaaa
Row 85 Size:  38
Pos 1: 0
Data:  85aaa
Pos 2: 5
Data:  a8585aaaaaaa
Row 86 Size:  38
Pos 1: 0
Data:  86aaa
Pos 2: 5
Data:  a8686aaaaaaa
Row 87 Size:  38
Pos 1: 0
Data:  87aaa
Pos 2: 5
Data:  a8787aaaaaaa
Row 88 Size:  38
Pos 1: 0
Data:  88aaa
Pos 2: 5
Data:  a8888aaaaaaa
Row 89 Size:  38
Pos 1: 0
Data:  89aaa
Pos 2: 5
Data:  a8989aaaaaaa
Row 90 Size:  38
Pos 1: 0
Data:  90aaa
Pos 2: 5
Data:  a9090aaaaaaa
Row 91 Size:  38
Pos 1: 0
Data:  91aaa
Pos 2: 5
Data:  a9191aaaaaaa
Row 92 Size:  38
Pos 1: 0
Data:  92aaa
Pos 2: 5
Data:  a9292aaaaaaa
Row 93 Size:  38
Pos 1: 0
Data:  93aaa
Pos 2: 5
Data:  a9393aaaaaaa
Row 94 Size:  38
Pos 1: 0
Data:  94aaa
Pos 2: 5
Data:  a9494aaaaaaa
Row 95 Size:  38
Pos 1: 0
Data:  95aaa
Pos 2: 5
Data:  a9595aaaaaaa
Row 96 Size:  38
Pos 1: 0
Data:  96aaa
Pos 2: 5
Data:  a9696aaaaaaa
Row 97 Size:  38
Pos 1: 0
Data:  97aaa
Pos 2: 5
Data:  a9797aaaaaaa
Row 98 Size:  38
Pos 1: 0
Data:  98aaa
Pos 2: 5
Data:  a9898aaaaaaa
Row 99 Size:  38
Pos 1: 0
Data:  99aaa
Pos 2: 5
Data:  a9999aaaaaaa
Row 100 Size:  41
Pos 1: 0
Data:  100aa
Pos 2: 5
Data:  aa100100aaaa
Row 101 Size:  41
Pos 1: 0
Data:  101aa
Pos 2: 5
Data:  aa101101aaaa
Row 102 Size:  41
Pos 1: 0
Data:  102aa
Pos 2: 5
Data:  aa102102aaaa
Row 103 Size:  41
Pos 1: 0
Data:  103aa
Pos 2: 5
Data:  aa103103aaaa
Row 104 Size:  41
Pos 1: 0
Data:  104aa
Pos 2: 5
Data:  aa104104aaaa
Row 105 Size:  41
Pos 1: 0
Data:  105aa
Pos 2: 5
Data:  aa105105aaaa
Row 106 Size:  41
Pos 1: 0
Data:  106aa
Pos 2: 5
Data:  aa106106aaaa
Row 107 Size:  41
Pos 1: 0
Data:  107aa
Pos 2: 5
Data:  aa107107aaaa
Row 108 Size:  41
Pos 1: 0
Data:  108aa
Pos 2: 5
Data:  aa108108aaaa
Row 109 Size:  41
Pos 1: 0
Data:  109aa
Pos 2: 5
Data:  aa109109aaaa
Row 110 Size:  41
Pos 1: 0
Data:  110aa
Pos 2: 5
Data:  aa110110aaaa
Row 111 Size:  41
Pos 1: 0
Data:  111aa
Pos 2: 5
Data:  aa111111aaaa
Row 112 Size:  41
Pos 1: 0
Data:  112aa
Pos 2: 5
Data:  aa112112aaaa
Row 113 Size:  41
Pos 1: 0
Data:  113aa
Pos 2: 5
Data:  aa113113aaaa
Row 114 Size:  41
Pos 1: 0
Data:  114aa
Pos 2: 5
Data:  aa114114aaaa
Row 115 Size:  41
Pos 1: 0
Data:  115aa
Pos 2: 5
Data:  aa115115aaaa
Row 116 Size:  41
Pos 1: 0
Data:  116aa
Pos 2: 5
Data:  aa116116aaaa
Row 117 Size:  41
Pos 1: 0
Data:  117aa
Pos 2: 5
Data:  aa117117aaaa
Row 118 Size:  41
Pos 1: 0
Data:  118aa
Pos 2: 5
Data:  aa118118aaaa
Row 119 Size:  41
Pos 1: 0
Data:  119aa
Pos 2: 5
Data:  aa119119aaaa
Row 120 Size:  41
Pos 1: 0
Data:  120aa
Pos 2: 5
Data:  aa120120aaaa
Row 121 Size:  41
Pos 1: 0
Data:  121aa
Pos 2: 5
Data:  aa121121aaaa
Row 122 Size:  41
Pos 1: 0
Data:  122aa
Pos 2: 5
Data:  aa122122aaaa
Row 123 Size:  41
Pos 1: 0
Data:  123aa
Pos 2: 5
Data:  aa123123aaaa
Row 124 Size:  41
Pos 1: 0
Data:  124aa
Pos 2: 5
Data:  aa124124aaaa
Row 125 Size:  41
Pos 1: 0
Data:  125aa
Pos 2: 5
Data:  aa125125aaaa
Row 126 Size:  41
Pos 1: 0
Data:  126aa
Pos 2: 5
Data:  aa126126aaaa
Row 127 Size:  41
Pos 1: 0
Data:  127aa
Pos 2: 5
Data:  aa127127aaaa
Row 128 Size:  41
Pos 1: 0
Data:  128aa
Pos 2: 5
Data:  aa128128aaaa
Row 129 Size:  41
Pos 1: 0
Data:  129aa
Pos 2: 5
Data:  aa129129aaaa
Row 130 Size:  41
Pos 1: 0
Data:  130aa
Pos 2: 5
Data:  aa130130aaaa
Row 131 Size:  41
Pos 1: 0
Data:  131aa
Pos 2: 5
Data:  aa131131aaaa
Row 132 Size:  41
Pos 1: 0
Data:  132aa
Pos 2: 5
Data:  aa132132aaaa
Row 133 Size:  41
Pos 1: 0
Data:  133aa
Pos 2: 5
Data:  aa133133aaaa
Row 134 Size:  41
Pos 1: 0
Data:  134aa
Pos 2: 5
Data:  aa134134aaaa
Row 135 Size:  41
Pos 1: 0
Data:  135aa
Pos 2: 5
Data:  aa135135aaaa
Row 136 Size:  41
Pos 1: 0
Data:  136aa
Pos 2: 5
Data:  aa136136aaaa
Row 137 Size:  41
Pos 1: 0
Data:  137aa
Pos 2: 5
Data:  aa137137aaaa
Row 138 Size:  41
Pos 1: 0
Data:  138aa
Pos 2: 5
Data:  aa138138aaaa
Row 139 Size:  41
Pos 1: 0
Data:  139aa
Pos 2: 5
Data:  aa139139aaaa
Row 140 Size:  41
Pos 1: 0
Data:  140aa
Pos 2: 5
Data:  aa140140aaaa
Row 141 Size:  41
Pos 1: 0
Data:  141aa
Pos 2: 5
Data:  aa141141aaaa
Row 142 Size:  41
Pos 1: 0
Data:  142aa
Pos 2: 5
Data:  aa142142aaaa
Row 143 Size:  41
Pos 1: 0
Data:  143aa
Pos 2: 5
Data:  aa143143aaaa
Row 144 Size:  41
Pos 1: 0
Data:  144aa
Pos 2: 5
Data:  aa144144aaaa
Row 145 Size:  41
Pos 1: 0
Data:  145aa
Pos 2: 5
Data:  aa145145aaaa
Row 146 Size:  41
Pos 1: 0
Data:  146aa
Pos 2: 5
Data:  aa146146aaaa
Row 147 Size:  41
Pos 1: 0
Data:  147aa
Pos 2: 5
Data:  aa147147aaaa
Row 148 Size:  41
Pos 1: 0
Data:  148aa
Pos 2: 5
Data:  aa148148aaaa
Row 149 Size:  41
Pos 1: 0
Data:  149aa
Pos 2: 5
Data:  aa149149aaaa
Row 150 Size:  41
Pos 1: 0
Data:  150aa
Pos 2: 5
Data:  aa150150aaaa
Row 151 Size:  41
Pos 1: 0
Data:  151aa
Pos 2: 5
Data:  aa151151aaaa
Row 152 Size:  41
Pos 1: 0
Data:  152aa
Pos 2: 5
Data:  aa152152aaaa
Row 153 Size:  41
Pos 1: 0
Data:  153aa
Pos 2: 5
Data:  aa153153aaaa
Row 154 Size:  41
Pos 1: 0
Data:  154aa
Pos 2: 5
Data:  aa154154aaaa
Row 155 Size:  41
Pos 1: 0
Data:  155aa
Pos 2: 5
Data:  aa155155aaaa
Row 156 Size:  41
Pos 1: 0
Data:  156aa
Pos 2: 5
Data:  aa156156aaaa
Row 157 Size:  41
Pos 1: 0
Data:  157aa
Pos 2: 5
Data:  aa157157aaaa
Row 158 Size:  41
Pos 1: 0
Data:  158aa
Pos 2: 5
Data:  aa158158aaaa
Row 159 Size:  41
Pos 1: 0
Data:  159aa
Pos 2: 5
Data:  aa159159aaaa
Row 160 Size:  41
Pos 1: 0
Data:  160aa
Pos 2: 5
Data:  aa160160aaaa
Row 161 Size:  41
Pos 1: 0
Data:  161aa
Pos 2: 5
Data:  aa161161aaaa
Row 162 Size:  41
Pos 1: 0
Data:  162aa
Pos 2: 5
Data:  aa162162aaaa
Row 163 Size:  41
Pos 1: 0
Data:  163aa
Pos 2: 5
Data:  aa163163aaaa
Row 164 Size:  41
Pos 1: 0
Data:  164aa
Pos 2: 5
Data:  aa164164aaaa
Row 165 Size:  41
Pos 1: 0
Data:  165aa
Pos 2: 5
Data:  aa165165aaaa
Row 166 Size:  41
Pos 1: 0
Data:  166aa
Pos 2: 5
Data:  aa166166aaaa
Row 167 Size:  41
Pos 1: 0
Data:  167aa
Pos 2: 5
Data:  aa167167aaaa
Row 168 Size:  41
Pos 1: 0
Data:  168aa
Pos 2: 5
Data:  aa168168aaaa
Row 169 Size:  41
Pos 1: 0
Data:  169aa
Pos 2: 5
Data:  aa169169aaaa
Row 170 Size:  41
Pos 1: 0
Data:  170aa
Pos 2: 5
Data:  aa170170aaaa
Row 171 Size:  41
Pos 1: 0
Data:  171aa
Pos 2: 5
Data:  aa171171aaaa
Row 172 Size:  41
Pos 1: 0
Data:  172aa
Pos 2: 5
Data:  aa172172aaaa
Row 173 Size:  41
Pos 1: 0
Data:  173aa
Pos 2: 5
Data:  aa173173aaaa
Row 174 Size:  41
Pos 1: 0
Data:  174aa
Pos 2: 5
Data:  aa174174aaaa
Row 175 Size:  41
Pos 1: 0
Data:  175aa
Pos 2: 5
Data:  aa175175aaaa
Row 176 Size:  41
Pos 1: 0
Data:  176aa
Pos 2: 5
Data:  aa176176aaaa
Row 177 Size:  41
Pos 1: 0
Data:  177aa
Pos 2: 5
Data:  aa177177aaaa
Row 178 Size:  41
Pos 1: 0
Data:  178aa
Pos 2: 5
Data:  aa178178aaaa
Row 179 Size:  41
Pos 1: 0
Data:  179aa
Pos 2: 5
Data:  aa179179aaaa
Row 180 Size:  41
Pos 1: 0
Data:  180aa
Pos 2: 5
Data:  aa180180aaaa
Row 181 Size:  41
Pos 1: 0
Data:  181aa
Pos 2: 5
Data:  aa181181aaaa
Row 182 Size:  41
Pos 1: 0
Data:  182aa
Pos 2: 5
Data:  aa182182aaaa
Row 183 Size:  41
Pos 1: 0
Data:  183aa
Pos 2: 5
Data:  aa183183aaaa
Row 184 Size:  41
Pos 1: 0
Data:  184aa
Pos 2: 5
Data:  aa184184aaaa
Row 185 Size:  41
Pos 1: 0
Data:  185aa
Pos 2: 5
Data:  aa185185aaaa
Row 186 Size:  41
Pos 1: 0
Data:  186aa
Pos 2: 5
Data:  aa186186aaaa
Row 187 Size:  41
Pos 1: 0
Data:  187aa
Pos 2: 5
Data:  aa187187aaaa
Row 188 Size:  41
Pos 1: 0
Data:  188aa
Pos 2: 5
Data:  aa188188aaaa
Row 189 Size:  41
Pos 1: 0
Data:  189aa
Pos 2: 5
Data:  aa189189aaaa
Row 190 Size:  41
Pos 1: 0
Data:  190aa
Pos 2: 5
Data:  aa190190aaaa
Row 191 Size:  41
Pos 1: 0
Data:  191aa
Pos 2: 5
Data:  aa191191aaaa
Row 192 Size:  41
Pos 1: 0
Data:  192aa
Pos 2: 5
Data:  aa192192aaaa
Row 193 Size:  41
Pos 1: 0
Data:  193aa
Pos 2: 5
Data:  aa193193aaaa
Row 194 Size:  41
Pos 1: 0
Data:  194aa
Pos 2: 5
Data:  aa194194aaaa
Row 195 Size:  41
Pos 1: 0
Data:  195aa
Pos 2: 5
Data:  aa195195aaaa
Row 196 Size:  41
Pos 1: 0
Data:  196aa
Pos 2: 5
Data:  aa196196aaaa
Row 197 Size:  41
Pos 1: 0
Data:  197aa
Pos 2: 5
Data:  aa197197aaaa
Row 198 Size:  41
Pos 1: 0
Data:  198aa
Pos 2: 5
Data:  aa198198aaaa
Row 199 Size:  41
Pos 1: 0
Data:  199aa
Pos 2: 5
Data:  aa199199aaaa
Done
