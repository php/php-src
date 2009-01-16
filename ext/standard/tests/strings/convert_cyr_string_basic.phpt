--TEST--
Test convert_cyr_string() function : basic functionality 
--FILE--
<?php

/* Prototype  : string convert_cyr_string  ( string $str  , string $from  , string $to  )
 * Description: Convert from one Cyrillic character set to another
 * Source code: ext/standard/string.c
*/

echo "*** Testing convert_cyr_string() : basic functionality ***\n";

$str = "Convert from one Cyrillic character set to another.";

echo "\n-- First try some simple English text --\n";
var_dump(bin2hex(convert_cyr_string($str, 'w', 'k')));
var_dump(bin2hex(convert_cyr_string($str, 'w', 'i')));


echo "\n-- Now try some of characters in 128-255 range --\n";

for ($i = 128; $i < 256; $i++) {
	$str = chr($i);
	echo "$i: " . bin2hex(convert_cyr_string($str, 'w', 'k')) . "\n";
}

?>
===DONE===
--EXPECTF--
*** Testing convert_cyr_string() : basic functionality ***

-- First try some simple English text --
string(102) "436f6e766572742066726f6d206f6e6520437972696c6c6963206368617261637465722073657420746f20616e6f746865722e"
string(102) "436f6e766572742066726f6d206f6e6520437972696c6c6963206368617261637465722073657420746f20616e6f746865722e"

-- Now try some of characters in 128-255 range --
128: 2e
129: 2e
130: 2e
131: 2e
132: 2e
133: 2e
134: 2e
135: 2e
136: 2e
137: 2e
138: 2e
139: 2e
140: 2e
141: 2e
142: 2e
143: 2e
144: 2e
145: 2e
146: 2e
147: 2e
148: 2e
149: 2e
150: 2e
151: 2e
152: 2e
153: 2e
154: 2e
155: 2e
156: 2e
157: 2e
158: 2e
159: 2e
160: 9a
161: ae
162: be
163: 2e
164: 9f
165: bd
166: 2e
167: 2e
168: b3
169: bf
170: b4
171: 9d
172: 2e
173: 2e
174: 9c
175: b7
176: 2e
177: 2e
178: b6
179: a6
180: ad
181: 2e
182: 2e
183: 9e
184: a3
185: 98
186: a4
187: 9b
188: 2e
189: 2e
190: 2e
191: a7
192: e1
193: e2
194: f7
195: e7
196: e4
197: e5
198: f6
199: fa
200: e9
201: ea
202: eb
203: ec
204: ed
205: ee
206: ef
207: f0
208: f2
209: f3
210: f4
211: f5
212: e6
213: e8
214: e3
215: fe
216: fb
217: fd
218: ff
219: f9
220: f8
221: fc
222: e0
223: f1
224: c1
225: c2
226: d7
227: c7
228: c4
229: c5
230: d6
231: da
232: c9
233: ca
234: cb
235: cc
236: cd
237: ce
238: cf
239: d0
240: d2
241: d3
242: d4
243: d5
244: c6
245: c8
246: c3
247: de
248: db
249: dd
250: df
251: d9
252: d8
253: dc
254: c0
255: d1
===DONE===
