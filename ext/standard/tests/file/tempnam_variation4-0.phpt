--TEST--
Test tempnam() function: usage variations - permissions(0000 to 0350) of dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for Windows');
}
// Skip if being run by root
$filename = dirname(__FILE__)."/is_readable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
}
unlink($filename);
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Trying to create the file in a dir with permissions from 0000 to 0350,
     Allowable permissions: files are expected to be created in the input dir
     Non-allowable permissions: files are expected to be created in '/tmp' dir
*/

echo "*** Testing tempnam() with dir of permissions from 0000 to 0350 ***\n";
$file_path = dirname(__FILE__);
$dir_name = $file_path."/tempnam_variation4";
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

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
0

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
1

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
2

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
3

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
4

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
5

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
6

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
7

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
10

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
11

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
12

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
13

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
14

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
15

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
16

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
17

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
20

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
21

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
22

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
23

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
24

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
25

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
26

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
27

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
30

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
31

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
32

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
33

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
34

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
35

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
36

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
37

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
40

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
41

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
42

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
43

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
44

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
45

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
46

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
47

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
50

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
51

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
52

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
53

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
54

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
55

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
56

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
57

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
60

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
61

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
62

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
63

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
64

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
65

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
66

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
67

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
70

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
71

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
72

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
73

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
74

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
75

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
76

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
77

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
100

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
101

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
102

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
103

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
104

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
105

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
106

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
107

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
110

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
111

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
112

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
113

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
114

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
115

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
116

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
117

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
120

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
121

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
122

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
123

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
124

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
125

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
126

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
127

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
130

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
131

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
132

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
133

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
134

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
135

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
136

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
137

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
140

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
141

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
142

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
143

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
144

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
145

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
146

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
147

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
150

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
151

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
152

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
153

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
154

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
155

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
156

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
157

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
160

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
161

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
162

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
163

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
164

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
165

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
166

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
167

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
170

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
171

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
172

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
173

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
174

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
175

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
176

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
177

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
200

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
201

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
202

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
203

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
204

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
205

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
206

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
207

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
210

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
211

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
212

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
213

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
214

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
215

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
216

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
217

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
220

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
221

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
222

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
223

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
224

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
225

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
226

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
227

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
230

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
231

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
232

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
233

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
234

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
235

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
236

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
237

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
240

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
241

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
242

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
243

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
244

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
245

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
246

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
247

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
250

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
251

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
252

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
253

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
254

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
255

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
256

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
257

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
260

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
261

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
262

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
263

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
264

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
265

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
266

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
267

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
270

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
271

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
272

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
273

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
274

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
275

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
276

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-0.php on line 20
277
*** Done ***
