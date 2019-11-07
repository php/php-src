--TEST--
Oracle Database 12c Implicit Result Sets: bigger data size
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle Database 12c or greater");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) {
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmt =
    "declare
       c1 sys_refcursor;
     begin
       open c1 for select 1 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 2 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 3 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 4 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 5 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 6 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 7 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 8 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 9 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 10 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 11 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 12 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 13 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 14 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 15 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 16 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 17 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 18 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 19 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 20 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 21 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 22 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 23 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 24 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 25 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 26 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 27 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 28 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 29 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 30 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 31 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 32 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 33 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 34 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 35 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 36 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 37 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 38 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 39 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 40 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 41 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 42 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 43 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 44 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 45 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 46 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 47 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 48 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 49 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 50 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 51 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 52 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 53 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 54 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 55 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 56 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 57 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 58 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 59 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 60 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 61 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 62 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 63 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 64 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 65 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 66 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 67 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 68 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 69 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 70 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 71 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 72 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 73 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 74 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 75 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 76 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 77 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 78 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 79 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 80 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 81 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 82 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 83 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 84 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 85 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 86 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 87 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 88 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 89 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 90 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 91 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 92 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 93 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 94 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 95 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 96 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 97 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 98 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 99 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 100 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 101 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 102 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 103 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 104 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 105 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 106 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 107 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 108 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 109 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 110 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 111 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 112 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 113 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 114 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 115 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 116 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 117 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 118 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 119 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 120 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 121 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 122 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 123 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 124 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 125 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 126 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 127 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 128 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 129 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 130 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 131 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 132 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 133 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 134 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 135 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 136 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 137 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 138 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 139 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 140 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 141 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 142 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 143 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 144 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 145 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 146 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 147 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 148 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 149 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 150 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 151 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 152 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 153 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 154 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 155 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 156 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 157 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 158 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 159 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 160 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 161 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 162 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 163 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 164 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 165 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 166 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 167 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 168 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 169 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 170 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 171 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 172 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 173 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 174 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 175 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 176 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 177 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 178 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 179 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 180 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 181 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 182 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 183 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 184 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 185 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 186 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 187 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 188 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 189 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 190 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 191 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 192 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 193 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 194 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 195 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 196 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 197 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 198 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 199 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 200 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 201 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 202 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 203 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 204 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 205 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 206 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 207 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 208 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 209 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 210 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 211 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 212 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 213 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 214 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 215 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 216 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 217 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 218 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 219 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 220 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 221 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 222 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 223 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 224 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 225 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 226 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 227 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 228 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 229 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 230 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 231 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 232 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 233 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 234 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 235 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 236 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 237 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 238 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 239 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 240 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 241 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 242 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 243 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 244 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 245 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 246 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 247 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 248 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 249 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 250 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 251 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 252 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 253 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 254 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 255 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 256 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 257 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 258 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 259 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 260 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 261 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 262 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 263 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 264 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 265 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 266 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 267 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 268 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 269 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 270 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 271 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 272 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 273 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 274 from dual;
       dbms_sql.return_result(c1);
       open c1 for select 275 from dual;
       dbms_sql.return_result(c1);
     end;";

// Run Test

echo "Test 1\n";
$s = oci_parse($c, $stmt);
oci_execute($s);

while (($row = oci_fetch_row($s)) != false) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

?>
--EXPECT--
Test 1
  1
  2
  3
  4
  5
  6
  7
  8
  9
  10
  11
  12
  13
  14
  15
  16
  17
  18
  19
  20
  21
  22
  23
  24
  25
  26
  27
  28
  29
  30
  31
  32
  33
  34
  35
  36
  37
  38
  39
  40
  41
  42
  43
  44
  45
  46
  47
  48
  49
  50
  51
  52
  53
  54
  55
  56
  57
  58
  59
  60
  61
  62
  63
  64
  65
  66
  67
  68
  69
  70
  71
  72
  73
  74
  75
  76
  77
  78
  79
  80
  81
  82
  83
  84
  85
  86
  87
  88
  89
  90
  91
  92
  93
  94
  95
  96
  97
  98
  99
  100
  101
  102
  103
  104
  105
  106
  107
  108
  109
  110
  111
  112
  113
  114
  115
  116
  117
  118
  119
  120
  121
  122
  123
  124
  125
  126
  127
  128
  129
  130
  131
  132
  133
  134
  135
  136
  137
  138
  139
  140
  141
  142
  143
  144
  145
  146
  147
  148
  149
  150
  151
  152
  153
  154
  155
  156
  157
  158
  159
  160
  161
  162
  163
  164
  165
  166
  167
  168
  169
  170
  171
  172
  173
  174
  175
  176
  177
  178
  179
  180
  181
  182
  183
  184
  185
  186
  187
  188
  189
  190
  191
  192
  193
  194
  195
  196
  197
  198
  199
  200
  201
  202
  203
  204
  205
  206
  207
  208
  209
  210
  211
  212
  213
  214
  215
  216
  217
  218
  219
  220
  221
  222
  223
  224
  225
  226
  227
  228
  229
  230
  231
  232
  233
  234
  235
  236
  237
  238
  239
  240
  241
  242
  243
  244
  245
  246
  247
  248
  249
  250
  251
  252
  253
  254
  255
  256
  257
  258
  259
  260
  261
  262
  263
  264
  265
  266
  267
  268
  269
  270
  271
  272
  273
  274
  275
