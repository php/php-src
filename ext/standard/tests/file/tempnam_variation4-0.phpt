--TEST--
Test tempnam() function: usage variations - permissions(0000 to 0350) of dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Trying to create the file in a dir with permissions from 0000 to 0350,
     Allowable permissions: files are expected to be created in the input dir
     Non-allowable permissions: files are expected to be created in '/tmp' dir
*/

echo "*** Testing tempnam() with dir of permissions from 0000 to 0350 ***\n";
$file_path = __DIR__;
$dir_name = $file_path."/tempnam_variation4-0";
$prefix = "tempnamVar4.";

mkdir($dir_name);

for($mode = 0000; $mode <= 0350; $mode++) {
  chmod($dir_name, $mode);
  $file_name = tempnam($dir_name, $prefix);

  if(file_exists($file_name) ) {
    if (dirname($file_name) != $dir_name) {
      /* Either there's a notice or error */
       printf("%o\n", $mode);

      if (realpath(dirname($file_name)) != realpath(sys_get_temp_dir())) {
         echo " created in unexpected dir\n";
      }
    }
    unlink($file_name);
  }
  else {
    print("FAILED: File is not created\n");
  }
}

rmdir($dir_name);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with dir of permissions from 0000 to 0350 ***

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
0

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
1

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
2

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
3

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
4

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
5

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
6

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
7

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
10

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
11

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
12

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
13

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
14

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
15

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
16

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
17

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
20

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
21

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
22

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
23

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
24

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
25

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
26

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
27

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
30

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
31

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
32

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
33

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
34

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
35

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
36

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
37

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
40

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
41

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
42

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
43

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
44

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
45

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
46

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
47

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
50

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
51

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
52

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
53

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
54

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
55

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
56

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
57

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
60

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
61

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
62

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
63

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
64

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
65

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
66

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
67

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
70

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
71

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
72

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
73

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
74

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
75

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
76

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
77

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
100

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
101

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
102

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
103

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
104

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
105

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
106

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
107

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
110

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
111

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
112

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
113

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
114

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
115

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
116

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
117

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
120

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
121

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
122

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
123

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
124

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
125

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
126

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
127

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
130

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
131

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
132

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
133

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
134

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
135

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
136

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
137

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
140

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
141

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
142

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
143

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
144

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
145

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
146

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
147

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
150

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
151

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
152

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
153

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
154

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
155

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
156

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
157

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
160

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
161

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
162

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
163

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
164

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
165

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
166

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
167

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
170

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
171

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
172

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
173

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
174

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
175

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
176

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
177

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
200

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
201

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
202

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
203

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
204

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
205

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
206

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
207

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
210

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
211

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
212

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
213

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
214

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
215

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
216

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
217

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
220

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
221

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
222

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
223

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
224

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
225

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
226

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
227

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
230

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
231

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
232

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
233

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
234

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
235

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
236

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
237

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
240

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
241

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
242

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
243

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
244

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
245

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
246

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
247

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
250

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
251

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
252

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
253

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
254

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
255

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
256

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
257

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
260

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
261

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
262

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
263

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
264

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
265

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
266

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
267

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
270

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
271

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
272

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
273

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
274

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
275

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
276

Notice: tempnam(): file created in the system's temporary directory in %s on line %d
277
*** Done ***
