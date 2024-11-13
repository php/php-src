--TEST--
GHSA-9fcc-425m-g385 - bypass CVE-2024-1874 - exhaustive suffix test
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die('skip Run only on Windows');
?>
--FILE--
<?php

$batch_file_content = <<<EOT
@echo off
powershell -Command "Write-Output '%0%'"
powershell -Command "Write-Output '%1%'"
EOT;
$batch_file_path = __DIR__ . '/ghsa-9fcc-425m-g385_003.bat';

file_put_contents($batch_file_path, $batch_file_content);

$descriptorspec = [STDIN, STDOUT, STDOUT];

for ($i = 1; $i <= 255; $i++) {
  echo "Testing $i\n";
  try {
    $proc = @proc_open([$batch_file_path . chr($i), "\"&notepad.exe"], $descriptorspec, $pipes, null, null, array("bypass_shell" => true));
    var_dump($proc);
    proc_close($proc);
  } catch (Error) {}
}

?>
--EXPECTF--
Testing 1
bool(false)
Testing 2
bool(false)
Testing 3
bool(false)
Testing 4
bool(false)
Testing 5
bool(false)
Testing 6
bool(false)
Testing 7
bool(false)
Testing 8
bool(false)
Testing 9
bool(false)
Testing 10
bool(false)
Testing 11
bool(false)
Testing 12
bool(false)
Testing 13
bool(false)
Testing 14
bool(false)
Testing 15
bool(false)
Testing 16
bool(false)
Testing 17
bool(false)
Testing 18
bool(false)
Testing 19
bool(false)
Testing 20
bool(false)
Testing 21
bool(false)
Testing 22
bool(false)
Testing 23
bool(false)
Testing 24
bool(false)
Testing 25
bool(false)
Testing 26
bool(false)
Testing 27
bool(false)
Testing 28
bool(false)
Testing 29
bool(false)
Testing 30
bool(false)
Testing 31
bool(false)
Testing 32
resource(%d) of type (process)
%s.bat 
"&notepad.exe
Testing 33
bool(false)
Testing 34
bool(false)
Testing 35
bool(false)
Testing 36
bool(false)
Testing 37
bool(false)
Testing 38
bool(false)
Testing 39
bool(false)
Testing 40
bool(false)
Testing 41
bool(false)
Testing 42
bool(false)
Testing 43
bool(false)
Testing 44
bool(false)
Testing 45
bool(false)
Testing 46
resource(%d) of type (process)
'"%s.bat."' is not recognized as an internal or external command,
operable program or batch file.
Testing 47
bool(false)
Testing 48
bool(false)
Testing 49
bool(false)
Testing 50
bool(false)
Testing 51
bool(false)
Testing 52
bool(false)
Testing 53
bool(false)
Testing 54
bool(false)
Testing 55
bool(false)
Testing 56
bool(false)
Testing 57
bool(false)
Testing 58
bool(false)
Testing 59
bool(false)
Testing 60
bool(false)
Testing 61
bool(false)
Testing 62
bool(false)
Testing 63
bool(false)
Testing 64
bool(false)
Testing 65
bool(false)
Testing 66
bool(false)
Testing 67
bool(false)
Testing 68
bool(false)
Testing 69
bool(false)
Testing 70
bool(false)
Testing 71
bool(false)
Testing 72
bool(false)
Testing 73
bool(false)
Testing 74
bool(false)
Testing 75
bool(false)
Testing 76
bool(false)
Testing 77
bool(false)
Testing 78
bool(false)
Testing 79
bool(false)
Testing 80
bool(false)
Testing 81
bool(false)
Testing 82
bool(false)
Testing 83
bool(false)
Testing 84
bool(false)
Testing 85
bool(false)
Testing 86
bool(false)
Testing 87
bool(false)
Testing 88
bool(false)
Testing 89
bool(false)
Testing 90
bool(false)
Testing 91
bool(false)
Testing 92
bool(false)
Testing 93
bool(false)
Testing 94
bool(false)
Testing 95
bool(false)
Testing 96
bool(false)
Testing 97
bool(false)
Testing 98
bool(false)
Testing 99
bool(false)
Testing 100
bool(false)
Testing 101
bool(false)
Testing 102
bool(false)
Testing 103
bool(false)
Testing 104
bool(false)
Testing 105
bool(false)
Testing 106
bool(false)
Testing 107
bool(false)
Testing 108
bool(false)
Testing 109
bool(false)
Testing 110
bool(false)
Testing 111
bool(false)
Testing 112
bool(false)
Testing 113
bool(false)
Testing 114
bool(false)
Testing 115
bool(false)
Testing 116
bool(false)
Testing 117
bool(false)
Testing 118
bool(false)
Testing 119
bool(false)
Testing 120
bool(false)
Testing 121
bool(false)
Testing 122
bool(false)
Testing 123
bool(false)
Testing 124
bool(false)
Testing 125
bool(false)
Testing 126
bool(false)
Testing 127
bool(false)
Testing 128
bool(false)
Testing 129
bool(false)
Testing 130
bool(false)
Testing 131
bool(false)
Testing 132
bool(false)
Testing 133
bool(false)
Testing 134
bool(false)
Testing 135
bool(false)
Testing 136
bool(false)
Testing 137
bool(false)
Testing 138
bool(false)
Testing 139
bool(false)
Testing 140
bool(false)
Testing 141
bool(false)
Testing 142
bool(false)
Testing 143
bool(false)
Testing 144
bool(false)
Testing 145
bool(false)
Testing 146
bool(false)
Testing 147
bool(false)
Testing 148
bool(false)
Testing 149
bool(false)
Testing 150
bool(false)
Testing 151
bool(false)
Testing 152
bool(false)
Testing 153
bool(false)
Testing 154
bool(false)
Testing 155
bool(false)
Testing 156
bool(false)
Testing 157
bool(false)
Testing 158
bool(false)
Testing 159
bool(false)
Testing 160
bool(false)
Testing 161
bool(false)
Testing 162
bool(false)
Testing 163
bool(false)
Testing 164
bool(false)
Testing 165
bool(false)
Testing 166
bool(false)
Testing 167
bool(false)
Testing 168
bool(false)
Testing 169
bool(false)
Testing 170
bool(false)
Testing 171
bool(false)
Testing 172
bool(false)
Testing 173
bool(false)
Testing 174
bool(false)
Testing 175
bool(false)
Testing 176
bool(false)
Testing 177
bool(false)
Testing 178
bool(false)
Testing 179
bool(false)
Testing 180
bool(false)
Testing 181
bool(false)
Testing 182
bool(false)
Testing 183
bool(false)
Testing 184
bool(false)
Testing 185
bool(false)
Testing 186
bool(false)
Testing 187
bool(false)
Testing 188
bool(false)
Testing 189
bool(false)
Testing 190
bool(false)
Testing 191
bool(false)
Testing 192
bool(false)
Testing 193
bool(false)
Testing 194
bool(false)
Testing 195
bool(false)
Testing 196
bool(false)
Testing 197
bool(false)
Testing 198
bool(false)
Testing 199
bool(false)
Testing 200
bool(false)
Testing 201
bool(false)
Testing 202
bool(false)
Testing 203
bool(false)
Testing 204
bool(false)
Testing 205
bool(false)
Testing 206
bool(false)
Testing 207
bool(false)
Testing 208
bool(false)
Testing 209
bool(false)
Testing 210
bool(false)
Testing 211
bool(false)
Testing 212
bool(false)
Testing 213
bool(false)
Testing 214
bool(false)
Testing 215
bool(false)
Testing 216
bool(false)
Testing 217
bool(false)
Testing 218
bool(false)
Testing 219
bool(false)
Testing 220
bool(false)
Testing 221
bool(false)
Testing 222
bool(false)
Testing 223
bool(false)
Testing 224
bool(false)
Testing 225
bool(false)
Testing 226
bool(false)
Testing 227
bool(false)
Testing 228
bool(false)
Testing 229
bool(false)
Testing 230
bool(false)
Testing 231
bool(false)
Testing 232
bool(false)
Testing 233
bool(false)
Testing 234
bool(false)
Testing 235
bool(false)
Testing 236
bool(false)
Testing 237
bool(false)
Testing 238
bool(false)
Testing 239
bool(false)
Testing 240
bool(false)
Testing 241
bool(false)
Testing 242
bool(false)
Testing 243
bool(false)
Testing 244
bool(false)
Testing 245
bool(false)
Testing 246
bool(false)
Testing 247
bool(false)
Testing 248
bool(false)
Testing 249
bool(false)
Testing 250
bool(false)
Testing 251
bool(false)
Testing 252
bool(false)
Testing 253
bool(false)
Testing 254
bool(false)
Testing 255
bool(false)
--CLEAN--
<?php
@unlink(__DIR__ . '/ghsa-9fcc-425m-g385_003.bat');
?>
